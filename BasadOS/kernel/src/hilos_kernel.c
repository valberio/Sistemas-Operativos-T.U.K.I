#include"hilos_kernel.h"	
	
	
 void crear_hilo(pthread_t hilo, int conexion_deseada,void*(*funcion_wrapper)(void*)){
    	Parametros_de_hilo parametros_hilo;
        parametros_hilo.conexion = conexion_deseada;
	    pthread_create(&hilo, NULL, funcion_wrapper, (void *)&parametros_hilo);
	    pthread_detach(hilo);

    }
    

void* crear_proceso_wrapper(void* arg) {
	Parametros_de_hilo* args = (Parametros_de_hilo *)arg;
    char* parametro = args->mensaje;
	int conexion = args->conexion;
	double estimacion = args->estimacion;
	int conexion_kernel_memoria = args->pid;
    crear_proceso(parametro, conexion, estimacion, conexion_kernel_memoria);
    return NULL;
}

void crear_proceso(char* codigo_recibido, int socket_consola, double estimado_inicial) {
	t_pcb* pcb = crear_pcb(codigo_recibido, socket_consola, estimado_inicial);
	

	//ACA VA TU LOGica
	log_info(logger, "Cant instr en creacion del pcb %i",  pcb->contexto_de_ejecucion->cant_instrucciones);
	sem_wait(&mutex_cola_new);
	queue_push(cola_new, pcb);
	sem_post(&mutex_cola_new);
	sem_post(&semaforo_de_procesos_para_ejecutar);
	log_info(logger, "Se crea el proceso %i en NEW", pcb->pid);
}


void *recibir_de_consolas_wrapper(void *arg) {
    Parametros_de_hilo *args = (Parametros_de_hilo *)arg;
    int parametro = args->conexion;
    recibir_de_consolas(parametro);
    return NULL;
}

void recibir_de_consolas(int server_consola) {
	int i = 0;

	while(true){
		int conexion_consola = esperar_cliente(server_consola);
		char* codigo_recibido = recibir_mensaje(conexion_consola);
		double estimado_de_rafaga = config_get_double_value(config, "ESTIMACION_INICIAL");
		pthread_t hilo_creador_de_proceso;
		Parametros_de_hilo parametros_hilo_crear_proceso;
		parametros_hilo_crear_proceso.mensaje = codigo_recibido;
		parametros_hilo_crear_proceso.conexion = conexion_consola;
		parametros_hilo_crear_proceso.estimacion = estimado_de_rafaga;
		pthread_create(&hilo_creador_de_proceso, NULL, crear_proceso_wrapper, (void*)&parametros_hilo_crear_proceso);
		pthread_join(hilo_creador_de_proceso, NULL);
		i++;
	}
}

void* manipulador_de_IO_wrapper(void* arg){
	Parametros_de_hilo *args = (Parametros_de_hilo *)arg;
    char* tiempo = args->mensaje;
	int pid = args->conexion;
    manipulador_de_IO(tiempo, pid);
    return NULL;
}

void manipulador_de_IO(char* tiempo_en_blocked, int pid){
	int tiempo = atoi(tiempo_en_blocked);
	sleep(tiempo);
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
	log_info(logger, "PID: %i - Estado Anterior: BLOCKED - Estado Actual: READY", pcb->pid);
	sem_post(&semaforo_procesos_en_ready);
}




