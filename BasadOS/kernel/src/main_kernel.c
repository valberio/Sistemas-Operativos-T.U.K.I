#include "main_kernel.h"

//LOS PUERTOS SIGUEN LA SIGUIENTE ASIGNACION: puerto_servidor_cliente

/*------------------------------------------------------------------*/
/*						CHECKPOINT 2								*/
/*------------------------------------------------------------------*/
/*			Planificación FIFO
			Planificación HRRN
			Manejo de recursos compartidos							*/
/*------------------------------------------------------------------*/

/*int main (void)
{
	--Ideas de diseño de kernel:
	//Un hilo que manda a los pcbs nuevos a la cola de ready
	//Un hilo que planifique el hilo siguiente a ejecutar (puedo tener un hilo fifo y un hilo
	//hrrn, con un switch de una variable de la config elijo cuál lanzar)
	//Un hilo que reciba por parámetro el pcb y lo ejecute

	//Idea para HRRN: que la "cola de ready" sea un diccionario, las keys son el pID,
	//los values son el PCB. El algortimo HRRN me devuelve el pid y busco el pcb en
	//el diccionario para ejecutarlo.

}
*/

//Esta variable se fija que haya por lo menos un elemento en la cola de new antes de pasar a mandarlos a 
//ready, habria que fijarse de resolver con una mejor solucion como semaforos o etc.
sem_t semaforo_de_procesos_para_ejecutar;
sem_t semaforo_procesos_en_ready;
sem_t mutex_cola_new;
sem_t mutex_cola_ready;
sem_t semaforo_multiprogramacion;

t_queue* cola_new;
t_queue* cola_ready;
t_queue* cola_blocked;
t_queue* cola_exit;

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

	sem_init(&semaforo_multiprogramacion, 0, multiprogramacion);
	sem_init(&semaforo_de_procesos_para_ejecutar, 0, 0);
	sem_init(&semaforo_procesos_en_ready, 0, 0);

	sem_init(&mutex_cola_new, 0, 1);
	sem_init(&mutex_cola_ready, 0, 1);


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
	
	/*Que deberia haber:
		-Cola de new
		-Cola de ready
		-Algoritmo FIFO que mande el primer proceso a Running, lo mande a la CPU

		while (hay instrucciones en ready)
		{
			//mando contexto
			//cpu devuelve el contexto actualizado
			//cpu devuelve un int que indica si pasar el proceso a exit o no
		}
	*/
	
	
	//terminar_programa(logger, config);
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
		pthread_t hilo_creador_de_proceso;
		Parametros_de_hilo parametros_hilo_crear_proceso;
		parametros_hilo_crear_proceso.mensaje = codigo_recibido;
		parametros_hilo_crear_proceso.conexion = conexion_consola;
		log_info(logger, "A el kernel ha llegado el proceso numero %d\n", i);
		pthread_create(&hilo_creador_de_proceso, NULL, crear_proceso_wrapper, (void*)&parametros_hilo_crear_proceso);
		pthread_join(hilo_creador_de_proceso, NULL);
		i++;
	}
}

void crear_proceso(char* codigo_recibido, int socket_consola) {
	t_pcb* pcb = crear_pcb(codigo_recibido, socket_consola);
	sem_wait(&mutex_cola_new);
	queue_push(cola_new, pcb);
	sem_post(&mutex_cola_new);
	sem_post(&semaforo_de_procesos_para_ejecutar);
}

void* crear_proceso_wrapper(void* arg) {
	Parametros_de_hilo* args = (Parametros_de_hilo *)arg;
    char* parametro = args->mensaje;
	int conexion = args->conexion;
    crear_proceso(parametro, conexion);
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

//t_pcb* salida_HRRN(){

//}


void administrar_procesos_de_ready(int cliente_cpu){
	while(cliente_cpu){
		//ESPERA A QUE HAYA POR LO MENOS 1 PROCESO EN READY	
		printf("ESPERANDO A QUE LLEGUE UN PROCESO A READY\n");
		sem_wait(&semaforo_procesos_en_ready);
		printf("PASE EL SEMAFORO DE READY\n");

		t_pcb* proceso_en_ejecucion;

		char* planificador = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
		
		/*if(strcmp(planificador, "HRRN") == 0){
		proceso_a_ejecutar = salida_HRRN();
		}*/

		if(strcmp(planificador, "FIFO") == 0){
		proceso_en_ejecucion = salida_FIFO();
		}

		//PLANIFICACION
		log_info(logger, "Saque de la cola de ready el proceso %i\n", proceso_en_ejecucion->pid);
		
		enviar_contexto_de_ejecucion(proceso_en_ejecucion->contexto_de_ejecucion, cliente_cpu);

		t_paquete* paquete = recibir_contexto_de_ejecucion(cliente_cpu);

		t_contexto_de_ejecucion* contexto_actualizado = malloc(sizeof(t_contexto_de_ejecucion));
		contexto_actualizado = deserializar_contexto_de_ejecucion(paquete->buffer);
		proceso_en_ejecucion->contexto_de_ejecucion = contexto_actualizado;

		log_info(logger, "Voy a ejecutar lo que recibi %i", paquete->codigo_operacion);
		switch(paquete->codigo_operacion)
		{
			
			case INTERRUPCION_A_READY: //Caso YIELD
				//Actualizo el Proceso_en_ejecucion y lo mando a ready
				sem_wait(&mutex_cola_ready);
				queue_push(cola_ready, proceso_en_ejecucion);
				sem_post(&mutex_cola_ready);

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
			default:
				break; 
		}
		
	}
}