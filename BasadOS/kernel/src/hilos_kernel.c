#include"hilos_kernel.h"	
	
	//HILO 1: ESPERA CONEXIONES DE CONSOLA
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
    crear_proceso(parametro, conexion, estimacion);
    return NULL;
}

void *recibir_de_consolas_wrapper(void *arg) {
    Parametros_de_hilo *args = (Parametros_de_hilo *)arg;
    int parametro = args->conexion;
    recibir_de_consolas(parametro);
    return NULL;
}


void* administrar_procesos_de_new_wrapper(void* arg){
	Parametros_de_hilo *args = (Parametros_de_hilo *)arg;
    int parametro = args->conexion;
    administrar_procesos_de_new(parametro);
    return NULL;
}
void* manipulador_de_IO_wrapper(void* arg){
	Parametros_de_hilo *args = (Parametros_de_hilo *)arg;
    char* tiempo = args->mensaje;
	int pid = args->conexion;
    manipulador_de_IO(tiempo, pid);
    return NULL;
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
	log_info(logger, "Se crea el proceso %i en NEW", pcb->pid);
}
