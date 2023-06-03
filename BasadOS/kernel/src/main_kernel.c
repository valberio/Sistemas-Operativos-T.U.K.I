#include "main_kernel.h"

/*------------------------------------------------------------------*/
/*						CHECKPOINT 2								*/
/*------------------------------------------------------------------*/
/*			Planificación FIFO---YA TAAAAA
			Planificación HRRN--- Ya estaaa
			LECTURA DE I/O---- YA ESTA
			Manejo de recursos compartidos -- listoooo							*/
/*------------------------------------------------------------------*/

//Esta variable se fija que haya por lo menos un elemento en la cola de new antes de pasar a mandarlos a 
//ready, habria que fijarse de resolver con una mejor solucion como semaforos o etc.
sem_t semaforo_de_procesos_para_ejecutar;
sem_t semaforo_procesos_en_ready;
sem_t semaforo_procesos_en_exit;
sem_t mutex_cola_new;
sem_t mutex_cola_ready;
sem_t mutex_cola_blocked;
sem_t mutex_cola_exit;
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

	cola_new = queue_create();
	cola_ready = queue_create();
	cola_blocked = queue_create();
	cola_exit = queue_create();
	
	int multiprogramacion = config_get_int_value(config, "GRADO_MAX_MULTIPROGRAMACION");	

	char** recursos_array = config_get_array_value(config, "RECURSOS");
	char** instancias_array = config_get_array_value(config, "INSTANCIAS_RECURSOS");
	
	recursos = list_create();
	crear_lista_de_recursos(recursos,recursos_array,instancias_array);
	
	

	sem_init(&semaforo_multiprogramacion, 0, multiprogramacion);
	sem_init(&semaforo_de_procesos_para_ejecutar, 0, 0);
	sem_init(&semaforo_procesos_en_ready, 0, 0);
	sem_init(&semaforo_procesos_en_exit, 0, 0);

	sem_init(&mutex_cola_new, 0, 1);
	sem_init(&mutex_cola_ready, 0, 1);
	sem_init(&mutex_cola_blocked, 0, 1);
	sem_init(&mutex_cola_exit, 0, 1);

	
	
	/*
	char* ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	char* puerto_memoria_kernel = config_get_string_value(config, "PUERTO_MEMORIA");

	int cliente_kernel = crear_conexion_al_server(logger, ip_memoria, puerto_memoria_kernel);

	 if (cliente_kernel)
	 {
	 	log_info(logger, "El kernel envió su conexión a la memoria!");
	 }
*/
	//Conecto el kernel como cliente a la CPU
	char* ip_cpu = config_get_string_value(config, "IP_CPU");
	char* puerto_cpu_kernel = config_get_string_value(config, "PUERTO_CPU");
	char* puerto_kernel_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
	int cliente_cpu = crear_conexion_al_server(logger, ip_cpu, puerto_cpu_kernel);
	int cliente_memoria = crear_conexion_al_server(logger, ip_cpu, puerto_kernel_memoria);
	
	// //Conecto el kernel como cliente del filesystem
	// char* ip_filesystem = config_get_string_value(config, "IP_FILESYSTEM");
	// char* puerto_filesystem_kernel = config_get_string_value(config, "PUERTO_FILESYSTEM");;
	// //int cliente_filesystem = crear_conexion_al_server(logger, ip_filesystem, puerto_filesystem_kernel);
	// if (cliente_filesystem)
	// {
	// 	log_info(logger, "El kernel envió su conexión al filesystem!");
	// }
	
	//Abro el server del kernel para recibir conexiones de la consola
	
	//CODIGO DE ESCUCHA
	char* ip_kernel = config_get_string_value(config, "IP");//HARCODEADO
	char* puerto_kernel_consola = config_get_string_value(config, "PUERTO_ESCUCHA");
	int server_consola = iniciar_servidor(logger, ip_kernel, puerto_kernel_consola);
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
	
	
	//HILO 4: ADMINISTRA PROCESOS EN EXIT
	pthread_t hilo_administrador_de_exit;
	pthread_create(&hilo_administrador_de_exit, NULL, administrar_procesos_de_exit, NULL);
	pthread_detach(hilo_administrador_de_exit);

	administrar_procesos_de_ready(cliente_cpu, cliente_memoria);

	terminar_programa(logger, config);
	liberar_conexion(cliente_cpu);
	liberar_conexion(server_consola);

	return EXIT_SUCCESS;
}


