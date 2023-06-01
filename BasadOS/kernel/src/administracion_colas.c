#include"administracion_colas.h"
void* administrar_procesos_de_exit(){
	while(1){
		sem_wait(&semaforo_procesos_en_exit);
		t_pcb* proceso_a_finalizar;
		t_paquete* paquete = crear_paquete();
		crear_buffer(paquete);
		paquete->codigo_operacion = 1;

		sem_wait(&mutex_cola_exit);
		proceso_a_finalizar = queue_pop(cola_exit);	
		sem_post(&mutex_cola_exit);		

		enviar_paquete(paquete, proceso_a_finalizar->socket_consola);
		log_info(logger, "Envio el mensaje de finalizacion a consola \n");
		sem_post(&semaforo_multiprogramacion);
	}
	return NULL;
}


void administrar_procesos_de_new(int cliente_cpu){
	while(cliente_cpu){
		
		sem_wait(&semaforo_de_procesos_para_ejecutar);
		
		sem_wait(&semaforo_multiprogramacion);

		sem_wait(&mutex_cola_new);
		t_pcb* nuevo_pcb = queue_pop(cola_new);
		sem_post(&mutex_cola_new);

		sem_wait(&mutex_cola_ready);
		time(&(nuevo_pcb->tiempo_de_llegada_a_ready)); //ACA EMPIEZA A CORRER SU TIEMPO EN READY
		queue_push(cola_ready, nuevo_pcb);
		sem_post(&mutex_cola_ready);

		sem_post(&semaforo_procesos_en_ready);
	}
}


void administrar_procesos_de_ready(int cliente_cpu, int cliente_memoria){
	while(cliente_cpu){
		//ESPERA A QUE HAYA POR LO MENOS 1 PROCESO EN READY	
		sem_wait(&semaforo_procesos_en_ready);

		t_pcb* proceso_en_ejecucion;
		char* planificador = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
		
		if(strcmp(planificador, "HRRN") == 0){
		proceso_en_ejecucion = salida_HRRN();
		}

		if(strcmp(planificador, "FIFO") == 0){
		proceso_en_ejecucion = salida_FIFO();
		}

		proceso_en_ejecucion->inicio_de_uso_de_cpu = clock();//ACA SE INICIALIZA EL TIEMPO EN EJECUCION
		
		//PLANIFICACION
		log_info(logger, "EL ESTIMADO DE RAFAGA %f\n", proceso_en_ejecucion->estimado_rafaga);
		enviar_contexto_de_ejecucion(proceso_en_ejecucion->contexto_de_ejecucion, cliente_cpu);

		int ejecucion = 1;

		while(ejecucion){
		t_paquete* paquete = recibir_contexto_de_ejecucion(cliente_cpu);
		

		t_contexto_de_ejecucion* contexto_actualizado = malloc(sizeof(t_contexto_de_ejecucion));
		contexto_actualizado = deserializar_contexto_de_ejecucion(paquete->buffer);
		proceso_en_ejecucion->contexto_de_ejecucion = contexto_actualizado;

		char* parametros_retorno;
		
		switch(paquete->codigo_operacion)
		{
			
			case INTERRUPCION_A_READY: //Caso YIELD
				//Actualizo el Proceso_en_ejecucion y lo mando a ready
				sem_wait(&mutex_cola_ready);
				proceso_en_ejecucion->fin_de_uso_de_cpu = clock();
				calcular_estimado_de_rafaga(proceso_en_ejecucion);
				queue_push(cola_ready, proceso_en_ejecucion);
				sem_post(&mutex_cola_ready);
				sem_post(&semaforo_procesos_en_ready);
				ejecucion = 0;
				break;

			case FINALIZACION: //Caso EXIT
				sem_wait(&mutex_cola_exit);
				queue_push(cola_exit, proceso_en_ejecucion);
				sem_post(&mutex_cola_exit);

				sem_post(&semaforo_procesos_en_exit);

				ejecucion = 0;
				//Actualizo el Proceso_en_ejecucion y lo mando a exit
				//Mando un mensaje a la consola del proceso avisándole que completó la ejecución
				//Mando un paquete con buffer vacio y código de operación EXIT
				
				break;

			case INTERRUPCION_BLOQUEANTE: //Caso I/O, tengo que recibir el tiempo que se bloquea el proceso
				parametros_retorno = recibir_mensaje(cliente_cpu);
				pthread_t hilo_procesos_IO[50];
				int i = 0;	

				Parametros_de_hilo parametros_IO;
				parametros_IO.mensaje = parametros_retorno;
				parametros_IO.conexion = proceso_en_ejecucion->pid;
				
				sem_wait(&mutex_cola_blocked);
				proceso_en_ejecucion->fin_de_uso_de_cpu = clock();
				calcular_estimado_de_rafaga(proceso_en_ejecucion);
				queue_push(cola_blocked, proceso_en_ejecucion);
				sem_post(&mutex_cola_blocked);
				
				pthread_create(&hilo_procesos_IO[i], NULL, manipulador_de_IO_wrapper, (void*)&parametros_IO);
				pthread_detach(hilo_procesos_IO[i]);
				i++;
				ejecucion = 0;
				break;
				
			case PETICION_RECURSO: //Caso WAIT, proceso pide un recurso
				parametros_retorno = recibir_mensaje(cliente_cpu);
				if(wait_recurso(parametros_retorno, proceso_en_ejecucion) == 0){
					enviar_mensaje("0", cliente_cpu);
				} else {
					enviar_mensaje("1", cliente_cpu);
					ejecucion = 0;		
				} break;


			case LIBERACION_RECURSO: //Caso SIGNAL, proceso libera un recurso
				parametros_retorno = recibir_mensaje(cliente_cpu);
				if(signal_recurso(parametros_retorno, proceso_en_ejecucion) == 0){
					enviar_mensaje("0", cliente_cpu);
				} else {
					enviar_mensaje("1", cliente_cpu);
					ejecucion = 0;
				}
				break;
			case CREAR_SEGMENTO:
				//CPU me pide que le pida a memoria que cree un segmento
				enviar_mensaje("Creame un segmento", cliente_memoria);
				break;

			case ELIMINAR_SEGMENTO:
				//CPU me pide que le pida a memoria que elimine un segmento
				enviar_mensaje("Eliminame un segmento", cliente_memoria);
				break;
			default:
				break; 
		}
		
	}
}}