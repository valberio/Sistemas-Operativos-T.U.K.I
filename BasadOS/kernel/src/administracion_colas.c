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

		sem_post(&semaforo_multiprogramacion);
	}
	return NULL;
}

void* administrar_procesos_de_new_wrapper(void* arg){
	Parametros_de_hilo *args = (Parametros_de_hilo *)arg;
    int parametro = args->conexion;
    administrar_procesos_de_new(parametro);
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

		log_info(logger, "PID: %i - Estado anterior: NEW - Estado actual: READY", nuevo_pcb->pid);

		sem_post(&semaforo_procesos_en_ready);
	}
}


void administrar_procesos_de_ready(int cliente_cpu, int cliente_memoria, int cliente_filesystem){
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

		log_info(logger, "PID: %i - Estado anterior: READY - Estado actual: RUNNING", proceso_en_ejecucion->pid);
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
				log_info(logger, "PID: %i - Estado anterior: RUNNING - Estado actual: READY", proceso_en_ejecucion->pid);
				ejecucion = 0;
				break;

			case FINALIZACION: //Caso EXIT

				sem_wait(&mutex_cola_exit);
				queue_push(cola_exit, proceso_en_ejecucion);
				sem_post(&mutex_cola_exit);

				sem_post(&semaforo_procesos_en_exit);
				log_info(logger, "PID: %i - Estado anterior: RUNNING - Estado actual: EXIT", proceso_en_ejecucion->pid);
				log_info(logger, "Finaliza el proceso %i - Motivo: SUCCES", proceso_en_ejecucion->pid);
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

				log_info(logger, "PID: %i - Ejecuta IO: %s", proceso_en_ejecucion->pid, parametros_retorno);
				log_info(logger, "PID: %i - Estado anterior: RUNNING - Estado actual: BLOCKED", proceso_en_ejecucion->pid);
				log_info(logger, "PID: %i - Bloqueado por: IO", proceso_en_ejecucion->pid);
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
				/*t_paquete* paquete_a_memoria = crear_paquete();
				paquete_a_memoria->codigo_operacion = CREAR_SEGMENTO;
				paquete->buffer = NULL;
				enviar_paquete(paquete_a_memoria, cliente_memoria);
				eliminar_paquete(paquete_a_memoria);*/
				enviar_mensaje("Creame un segmento", cliente_memoria);
				break;

			case ELIMINAR_SEGMENTO:
				//CPU me pide que le pida a memoria que elimine un segmento
				/*t_paquete* paquete_a_memoria1 = crear_paquete();
				paquete_a_memoria1->codigo_operacion = ELIMINAR_SEGMENTO;
				paquete->buffer = NULL;
				enviar_paquete(paquete_a_memoria1, cliente_memoria);
				eliminar_paquete(paquete_a_memoria1);*/
				enviar_mensaje("Eliminame un segmento", cliente_memoria);
				break;

			case ABRIR_ARCHIVO:
				//ENVIO EL TIPO DE OPERACION AL FS
				parametros_retorno = recibir_mensaje(cliente_cpu);
				int buscar_archivo = buscar_archivo_en_tabla_global(parametros_retorno);
				if(buscar_archivo == 0) {//caso de que el archivo no estuviese abierto
					//Creo la estructura del archivo
					Archivo* nuevo_archivo = crear_archivo(parametros_retorno);
					list_add(lista_archivos_abiertos, nuevo_archivo);
					//Y la añado a la lista de archivos abiertos global
					t_paquete* paquete = crear_paquete();
    				paquete->codigo_operacion = ABRIR_ARCHIVO;
    				enviar_paquete(paquete, cliente_filesystem);
					//ENVIO EL NOMBRE DEL ARCHIVO REQUERIDO
					enviar_mensaje(parametros_retorno, cliente_filesystem);
					enviar_mensaje("0", cliente_cpu);
				} else {
					log_info(logger, "El archivo esta en uso");
					aniadir_a_bloqueados(proceso_en_ejecucion, parametros_retorno);
					log_info(logger, "PID: %i - Estado anterior: RUNNING - Estado actual: BLOCKED", proceso_en_ejecucion->pid);
					ejecucion = 0;
				}
				break;
			default:
				break; 
		}
		
	}
}}

Archivo* crear_archivo(char* nombre_archivo){
	Archivo* archivo = malloc(sizeof(Archivo));
	archivo->nombre_archivo = malloc(sizeof(nombre_archivo));
	strcpy(archivo->nombre_archivo, nombre_archivo);
	archivo->posicion_puntero = 0;
	archivo->procesos_bloqueados = queue_create();
	return archivo;
}

int buscar_archivo_en_tabla_global(char* nombre_archivo) {
	t_list* lista_con_archivo = list_create();
	bool existe_el_archivo(void* elemento){
		Archivo* archivo_en_tabla;
		archivo_en_tabla = elemento;

		return strcmp(archivo_en_tabla->nombre_archivo, nombre_archivo) == 0;
	}
	lista_con_archivo = list_filter(lista_archivos_abiertos, existe_el_archivo);
	if (list_size(lista_con_archivo) == 0){
		return 0;
	} else {
		return 1;
	}
}

void aniadir_a_bloqueados(t_pcb* proceso, char* nombre_archivo) {
	bool existe_el_archivo(void* elemento){
		Archivo* archivo_en_tabla;
		archivo_en_tabla = elemento;

		return strcmp(archivo_en_tabla->nombre_archivo, nombre_archivo) == 0;
	}
	Archivo* archivo = list_find(lista_archivos_abiertos, existe_el_archivo);
	queue_push(archivo->procesos_bloqueados, proceso);
}