#include "main_kernel.h"

//LOS PUERTOS SIGUEN LA SIGUIENTE ASIGNACION: puerto_servidor_cliente

/*------------------------------------------------------------------*/
/*						CHECKPOINT 2								*/
/*------------------------------------------------------------------*/
/*	Levanta el archivo de configuración: HECHO!
	Se conecta a CPU, Memoria y File System: HECHO!
	Espera conexiones de las consolas: wip, habría que implementar
									   concurrencia
	Recibe de las consolas las instrucciones y arma el PCB: wip,falta el PCB
	Planificación de procesos con FIFO: un poquito de avance					*/
/*------------------------------------------------------------------*/

/*int main (void)
{
	// hilo de escucha a consola ---- mete procesos nuevos en new

	//Colas, guardamos PCBs en las colas
	cola new
	cola ready
	running es un estado, los estados son un enum
	cola exit (porque puede que un proceso termine mientras otro este usando exit)
	cola blocked 

	1 hilo que escuche las consolas
	|-> lanza un hilo que mete el proceso en new
		|-> lanza un hilo que mete el proceso en ready
			|-> 
	//Idea para HRRN: que la "cola de ready" sea un diccionario, las keys son el pID,
	//los values son el PCB. El algortimo HRRN me devuelve el pid y busco el pcb en
	//el diccionario para ejecutarlo.
}
*/

//Esta variable se fija que haya por lo menos un elemento en la cola de new antes de pasar a mandarlos a 
//ready, habria que fijarse de resolver con una mejor solucion como semaforos o etc.
sem_t semaforo_de_procesos_para_ejecutar;
sem_t semaforo_cola_new;

t_queue* cola_new;
t_queue* cola_ready;
t_queue* cola_blocked;
t_queue* cola_exit;

//Creacion del log
t_log* logger;

int main(void)
{
	char* ip;
	cola_new = queue_create();
	cola_ready = queue_create();
	cola_blocked = queue_create();
	cola_exit = queue_create();
	
	sem_init(&semaforo_de_procesos_para_ejecutar, 0, 0);
	sem_init(&semaforo_cola_new, 0, 1);

	logger = iniciar_logger("log_kernel.log", "LOG_KERNEL");
	t_config* config = iniciar_config("configs/config_kernel.config");
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

	//HILO 2	
	/*Parametros_de_hilo parametros_hilo_kernel_cpu;
	parametros_hilo_kernel_cpu.conexion = cliente_cpu;
	pthread_t hilo_administrador_de_ready;
	pthread_create(&hilo_administrador_de_ready, NULL, recibir_de_consolas_wrapper, (void *)&parametros_hilo_kernel_cpu);
	pthread_join(hilo_administrador_de_ready, NULL);*/
	
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
		pthread_t hilo_creador_de_proceso[50];
		Parametros_de_hilo parametros_hilo_crear_proceso;
		parametros_hilo_crear_proceso.mensaje = codigo_recibido;
		parametros_hilo_crear_proceso.conexion = conexion_consola;
		pthread_create(&hilo_creador_de_proceso[i], NULL, crear_proceso_wrapper, (void*)&parametros_hilo_crear_proceso);
		pthread_detach(hilo_creador_de_proceso[i]);
		log_info(logger, "A el kernel ha llegado el proceso numero %d\n", i);
		i++;
	}
}

void crear_proceso(char* codigo_recibido, int socket_consola) {
	t_pcb* pcb = crear_pcb(codigo_recibido, socket_consola);

	/*for (int i = 0; i < pcb->contexto_de_ejecucion->cant_instrucciones; i++)
	{
		printf("Largo instruccion %i\n", pcb->contexto_de_ejecucion->largo_instruccion[i]);
	}*/
	//printf("La primera ins es: %s\n", (char*)list_get(pcb->contexto_de_ejecucion.instrucciones, 0));
	sem_wait(&semaforo_cola_new);
	queue_push(cola_new, pcb);
	sem_post(&semaforo_cola_new);
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


void administrar_procesos_de_ready(int cliente_cpu){
	//no tiene que ser un break
	while(cliente_cpu){
		//ESPERA A RECIBIR POR LO MENOS 1 PROCESO
		
		sem_wait(&semaforo_de_procesos_para_ejecutar);
		
		sem_wait(&semaforo_cola_new);
		if (queue_size(cola_new) == 0)
		{
			log_info(logger, "No hay procesos en la cola de new");
			cliente_cpu = 0;
			close(cliente_cpu);
		} else {
		t_pcb* nuevo_pcb = queue_pop(cola_new);

		sem_post(&semaforo_cola_new);

		queue_push(cola_ready, nuevo_pcb);
		
		
		t_pcb* pcb = queue_pop(cola_ready);

		log_info(logger, "Saque de la cola de ready el proceso %i\n", pcb->pid);
		log_info(logger, "En el contexto hay %i", pcb->pid);//esto esta mal por que siempre muestra la primera
		
		enviar_contexto_de_ejecucion(pcb->contexto_de_ejecucion, cliente_cpu);

		t_paquete* paquete = recibir_contexto_de_ejecucion(cliente_cpu);

		t_contexto_de_ejecucion* contexto_actualizado = malloc(sizeof(t_contexto_de_ejecucion));
		contexto_actualizado = deserializar_contexto_de_ejecucion(paquete->buffer);
		pcb->contexto_de_ejecucion = contexto_actualizado;

		switch(paquete->codigo_operacion)
		{
			
			case INTERRUPCION_A_READY: //Caso YIELD

				//Actualizo el PCB y lo mando a ready
				queue_push(cola_ready, pcb);
				sem_post(&semaforo_de_procesos_para_ejecutar);
				
				break;

			case FINALIZACION: //Caso EXIT
				queue_push(cola_exit, pcb);
				//Actualizo el PCB y lo mando a exit
				break;
			default:
				break; 
		}
		}
		//Hay que hacer que no reviente si no recibe contexto, manejar el error
		//log_info(logger, "Recibi el contexto actualizado");
		//log_info(logger, "En el contexto hay %i", contexto_actualizado->program_counter);
	}
}