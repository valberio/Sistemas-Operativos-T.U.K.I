#include "main_kernel.h"

/*------------------------------------------------------------------*/
/*						CHECKPOINT 2								*/
/*------------------------------------------------------------------*/
/*			Planificación FIFO---YA TAAAAA
			Planificación HRRN--- Ya estaaa
			LECTURA DE I/O---- YA ESTA
			Manejo de recursos compartidos							*/
/*------------------------------------------------------------------*/

//Esta variable se fija que haya por lo menos un elemento en la cola de new antes de pasar a mandarlos a 
//ready, habria que fijarse de resolver con una mejor solucion como semaforos o etc.
sem_t semaforo_de_procesos_para_ejecutar;
sem_t semaforo_procesos_en_ready;
sem_t mutex_cola_new;
sem_t mutex_cola_ready;
sem_t mutex_cola_blocked;
sem_t semaforo_multiprogramacion;

t_queue* cola_new;
t_queue* cola_ready;
t_queue* cola_blocked;
t_queue* cola_exit;

size_t cantidad_recursos;
t_list* recursos;
//Creacion del log
t_log* logger;
t_config* config;

int main(void)
{
	logger = iniciar_logger("log_kernel.log", "LOG_KERNEL");
	config = iniciar_config("configs/config_kernel.config");

	char* ip;
	cola_new = queue_create();
	cola_ready = queue_create();
	cola_blocked = queue_create();
	cola_exit = queue_create();
	
	int multiprogramacion = config_get_int_value(config, "GRADO_MAX_MULTIPROGRAMACION");	
/*
	char** recursos_array = config_get_array_value(config, "RECURSOS");
	char** instancias_array = config_get_array_value(config, "INSTANCIAS_RECURSOS");
	size_t cantidad_recursos = contarCadenas(recursos_array);
	Recurso* recurso[cantidad_recursos];
	Recurso* temp;
	
	printf("LA CANTIDAD DE RECURSOS SON: %zu \n",cantidad_recursos);
	for(int i = 0; i < cantidad_recursos; i++){
		recurso[i] = malloc(sizeof(Recurso));
		recurso[i]->recurso = malloc(sizeof(recursos_array[i]));
		recurso[i]->recurso = recursos_array[i];
		recurso[i]->instancias = atoi(instancias_array[i]);
		recurso[i]->cola_de_bloqueados = queue_create();
		list_add(recursos, recurso[i]);
		
		temp = list_get(recursos, i);
		printf("LOS RECURSOS SON %s \n", temp->recurso);
		printf("SU CANTIDAD DE INSTANCIAS ES %d \n",  temp->instancias);
	}
	*/

	sem_init(&semaforo_multiprogramacion, 0, multiprogramacion);
	sem_init(&semaforo_de_procesos_para_ejecutar, 0, 0);
	sem_init(&semaforo_procesos_en_ready, 0, 0);

	sem_init(&mutex_cola_new, 0, 1);
	sem_init(&mutex_cola_ready, 0, 1);
	sem_init(&mutex_cola_blocked, 0, 1);


	ip = config_get_string_value(config, "IP");
	
	/*char* puerto_memoria_kernel = config_get_string_value(config, "PUERTO_MEMORIA");

	int cliente_kernel = crear_conexion_al_server(logger, ip, puerto_memoria_kernel);

	 if (cliente_kernel)
	 {
	 	log_info(logger, "El kernel envió su conexión a la memoria!");
	 }
*/
	//Conecto el kernel como cliente a la CPU
	char* puerto_cpu_kernel = config_get_string_value(config, "PUERTO_CPU");
	int cliente_cpu = crear_conexion_al_server(logger, ip, puerto_cpu_kernel);
	
	// //Conecto el kernel como cliente del filesystem
	// char* puerto_filesystem_kernel = config_get_string_value(config, "PUERTO_FILESYSTEM");;
	// //int cliente_filesystem = crear_conexion_al_server(logger, ip, puerto_filesystem_kernel);
	// if (cliente_filesystem)
	// {
	// 	log_info(logger, "El kernel envió su conexión al filesystem!");
	// }
	
	//Abro el server del kernel para recibir conexiones de la consola
	
	//CODIGO DE ESCUCHA

	char* puerto_kernel_consola = config_get_string_value(config, "PUERTO_CONSOLA");
	int server_consola = iniciar_servidor(logger, ip, puerto_kernel_consola);
	if (server_consola != -1)
	{
		log_info(logger, "El servidor del kernel se inició");
	}
	
	//HILO 1: ESPERA CONEXIONES DE CONSOLA
	Parametros_de_hilo parametros_hilo_consola_kernel;
	parametros_hilo_consola_kernel.conexion = server_consola;
	pthread_t hilo_receptor_de_consolas;
	pthread_create(&hilo_receptor_de_consolas, NULL, recibir_de_consolas_wrapper, (void *)&parametros_hilo_consola_kernel);
	pthread_detach(hilo_receptor_de_consolas);

	//HILO 2: ADMINISTRA PROCESOS DE NEW	
	Parametros_de_hilo parametros_hilo_procesos_new;
	parametros_hilo_procesos_new.conexion = cliente_cpu;
	pthread_t hilo_administrador_de_new;
	pthread_create(&hilo_administrador_de_new, NULL, administrar_procesos_de_new_wrapper , (void *)&parametros_hilo_procesos_new);
	pthread_detach(hilo_administrador_de_new);
	
	administrar_procesos_de_ready(cliente_cpu);
	
	
	terminar_programa(logger, config);
	return EXIT_SUCCESS;
}



void terminar_programa(t_log* logger, t_config* config)
{
	 //Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) 
	 // con las funciones de las commons y del TP mencionadas en el enunciado 
	if(logger != NULL){
		log_destroy(logger);
	}
	if(config != NULL){
		config_destroy(config);
	}
}


void recibir_de_consolas(int server_consola) {
	int i = 0;
	while(true){
		int conexion_consola = esperar_cliente(server_consola);
		char* codigo_recibido = recibir_mensaje(conexion_consola);
		log_info(logger, "El kernel recibió el mensaje de consola");
		double estimado_de_rafaga = config_get_double_value(config, "ESTIMACION_INICIAL");
		pthread_t hilo_creador_de_proceso;
		Parametros_de_hilo parametros_hilo_crear_proceso;
		parametros_hilo_crear_proceso.mensaje = codigo_recibido;
		parametros_hilo_crear_proceso.conexion = conexion_consola;
		parametros_hilo_crear_proceso.estimacion = estimado_de_rafaga;
		log_info(logger, "A el kernel ha llegado el proceso numero %d\n", i);
		pthread_create(&hilo_creador_de_proceso, NULL, crear_proceso_wrapper, (void*)&parametros_hilo_crear_proceso);
		pthread_join(hilo_creador_de_proceso, NULL);
		i++;
	}
}

void crear_proceso(char* codigo_recibido, int socket_consola, double estimado_inicial) {
	t_pcb* pcb = crear_pcb(codigo_recibido, socket_consola, estimado_inicial);
	log_info(logger, "Cant instr en creacion del pcb %i",  pcb->contexto_de_ejecucion->cant_instrucciones);
	sem_wait(&mutex_cola_new);
	queue_push(cola_new, pcb);
	sem_post(&mutex_cola_new);
	sem_post(&semaforo_de_procesos_para_ejecutar);
}

void* crear_proceso_wrapper(void* arg) {
	Parametros_de_hilo* args = (Parametros_de_hilo *)arg;
    char* parametro = args->mensaje;
	int conexion = args->conexion;
	double estimacion = args->estimacion;
    crear_proceso(parametro, conexion, estimacion);
    return NULL;
}

void *recibir_de_consolas_wrapper(void *arg) {
    Parametros_de_hilo *args = (Parametros_de_hilo *)arg;
    int parametro = args->conexion;
    recibir_de_consolas(parametro);
    return NULL;
}


void administrar_procesos_de_new(int cliente_cpu){
	while(cliente_cpu){
		printf("ESPERANDO A QUE LLEGUE UN PROCESO A NEW\n");
		sem_wait(&semaforo_de_procesos_para_ejecutar);
		printf("PASE EL SEMAFORO DE NEW\n");

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

void* administrar_procesos_de_new_wrapper(void* arg){
	Parametros_de_hilo *args = (Parametros_de_hilo *)arg;
    int parametro = args->conexion;
    administrar_procesos_de_new(parametro);
    return NULL;
}

t_pcb* salida_FIFO(){
	sem_wait(&mutex_cola_ready);
	t_pcb* pcb = queue_pop(cola_ready);
	sem_post(&mutex_cola_ready);
	return pcb;
}

void calcular_estimado_de_rafaga(t_pcb* pcb){
	double alfa = config_get_double_value(config, "HRRN_ALFA");
	pcb->tiempo_de_la_ultima_rafaga = ((double) (pcb->fin_de_uso_de_cpu - pcb->inicio_de_uso_de_cpu) / CLOCKS_PER_SEC )* 1000;
	pcb->estimado_rafaga = alfa * pcb->tiempo_de_la_ultima_rafaga + (1-alfa) * pcb->estimado_rafaga;
} // esto esta bien

t_pcb* salida_HRRN(){
	t_pcb* pcb;
	t_pcb* pcb_hrr;
	time_t tiempo;
	double tiempo_esperando_en_ready, highest_response_ratio;
	int indice;
	highest_response_ratio = 0;
	time(&tiempo);
	for(int i = 0; i< queue_size(cola_ready); i ++){
		pcb = list_get(cola_ready->elements, i);
		tiempo_esperando_en_ready = difftime(pcb->tiempo_de_llegada_a_ready,tiempo);
		if((tiempo_esperando_en_ready + pcb->estimado_rafaga) / pcb->estimado_rafaga > highest_response_ratio){
			highest_response_ratio = (tiempo_esperando_en_ready + pcb->estimado_rafaga) / pcb->estimado_rafaga;
			pcb_hrr = list_get(cola_ready->elements, i);
			indice = i;
		}
	}
	
	// list_sort(cola_ready->elements, (tiempo_esperando_en_ready + pcb->estimado_rafaga) / pcb->estimado_rafaga > highest_response_ratio) DEBERIA FUNCIONAR
	// queue_pop(cola_ready) DEBERIA FUNCIONAR
	list_remove(cola_ready->elements,indice);
	return pcb_hrr;
}



void administrar_procesos_de_ready(int cliente_cpu){
	while(cliente_cpu){
		//ESPERA A QUE HAYA POR LO MENOS 1 PROCESO EN READY	
		printf("ESPERANDO A QUE LLEGUE UN PROCESO A READY\n");
		sem_wait(&semaforo_procesos_en_ready);
		printf("PASE EL SEMAFORO DE READY\n");

		t_pcb* proceso_en_ejecucion;
		char* planificador = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
		
		if(strcmp(planificador, "HRRN") == 0){
		proceso_en_ejecucion = salida_HRRN();
		printf("EL PID DE PROCESO EN EJECUCION EN HRRN ES %i", proceso_en_ejecucion->pid);
		printf("EL PID DE PROCESO EN EJECUCION EN HRRN ES %i", proceso_en_ejecucion->contexto_de_ejecucion->pid);
		}

		if(strcmp(planificador, "FIFO") == 0){
		proceso_en_ejecucion = salida_FIFO();
		printf("EL PID DE PROCESO EN EJECUCION EN FIFO ES %i", proceso_en_ejecucion->pid);

		}

		proceso_en_ejecucion->inicio_de_uso_de_cpu = clock();//ACA SE INICIALIZA EL TIEMPO EN EJECUCION
		
		//PLANIFICACION
		log_info(logger, "Saque de la cola de ready el proceso %i\n", proceso_en_ejecucion->pid);
		printf("EL ESTIMADO DE RAFAGA %f\n", proceso_en_ejecucion->estimado_rafaga);
		enviar_contexto_de_ejecucion(proceso_en_ejecucion->contexto_de_ejecucion, cliente_cpu);

		t_paquete* paquete = recibir_contexto_de_ejecucion(cliente_cpu);
		

		t_contexto_de_ejecucion* contexto_actualizado = malloc(sizeof(t_contexto_de_ejecucion));
		contexto_actualizado = deserializar_contexto_de_ejecucion(paquete->buffer);
		proceso_en_ejecucion->contexto_de_ejecucion = contexto_actualizado;

		log_info(logger, "Voy a ejecutar lo que recibi %i", paquete->codigo_operacion);

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
				printf("EL ESTIMADO NUEVO ES %f\n", proceso_en_ejecucion->estimado_rafaga);
				sem_post(&semaforo_procesos_en_ready);
				
				break;

			case FINALIZACION: //Caso EXIT
				queue_push(cola_exit, proceso_en_ejecucion);
				//Actualizo el Proceso_en_ejecucion y lo mando a exit
				//Mando un mensaje a la consola del proceso avisándole que completó la ejecución
				//Mando un paquete con buffer vacio y código de operación EXIT
				t_paquete* paquete = crear_paquete();
				crear_buffer(paquete);
				paquete->codigo_operacion = 1;
				
				enviar_paquete(paquete, proceso_en_ejecucion->socket_consola);
				log_info(logger, "Envio el mensaje de finalizacion a consola \n");
				sem_post(&semaforo_multiprogramacion);
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
				break;
				
			case PETICION_RECURSO: //Caso WAIT, proceso pide un recurso
				parametros_retorno = recibir_mensaje(cliente_cpu);
				break;
			case LIBERACION_RECURSO: //Caso SIGNAL, proceso libera un recurso
				parametros_retorno = recibir_mensaje(cliente_cpu);
				break;
			default:
				break; 
		}
		
	}
}

void manipulador_de_IO(char* tiempo_en_blocked, int pid){
	int tiempo = atoi(tiempo_en_blocked);
	time_t tiempo_de_inicio,tiempo_de_salida;
	time(&tiempo_de_inicio);
	printf("EMPIEZO A DORMIR\n");
	sleep(tiempo);
	time(&tiempo_de_salida);
	double tiempo_final = difftime(tiempo_de_salida, tiempo_de_inicio);
	printf("TERMINO DE DORMIR, TARDE %f\n", tiempo_final);

	t_pcb* pcb;

	bool buscar_proceso(void* elemento){
		return buscar_pid(elemento, pid);
	}
	
	sem_wait(&mutex_cola_blocked);	
	pcb = list_get(list_filter(cola_blocked->elements, buscar_proceso),0);
	list_remove_by_condition(cola_blocked->elements, buscar_proceso);	
	sem_post(&mutex_cola_blocked);

	sem_wait(&mutex_cola_ready);
	queue_push(cola_ready, pcb);
	sem_post(&mutex_cola_ready);

	sem_post(&semaforo_procesos_en_ready);
}

bool buscar_pid(void* pcb, int pid){
	return ((t_pcb* )pcb)->pid == pid;
}

void* manipulador_de_IO_wrapper(void* arg){
	Parametros_de_hilo *args = (Parametros_de_hilo *)arg;
    char* tiempo = args->mensaje;
	int pid = args->conexion;
    manipulador_de_IO(tiempo, pid);
    return NULL;
}



// void wait_recurso(char* recurso){
// 	for(int i = 0, i < cantidad_recursos, i++){
// 		if(recursos[i].recurso == recurso)
// 	}
// }

size_t contarCadenas(char** array) {
    size_t contador = 0;

    while (array[contador] != NULL) {
        contador++;
    }

    return contador;
}
