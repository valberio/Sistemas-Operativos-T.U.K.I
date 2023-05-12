#include "main_kernel.h"
#include <pthread.h>
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



int main(void)
{
	char* ip;
	//pthread_t hilo_de_escucha;
	//pthread_create(&hilo_de_escucha, NULL, (void*)conectarse_con_consola, (void*) &params);

	t_log* logger = iniciar_logger("log_kernel.log", "LOG_KERNEL");
	t_config* config = iniciar_config("configs/config_kernel.config");
	

	ip = config_get_string_value(config, "IP");
	
	/*char* puerto_memoria_kernel = config_get_string_value(config, "PUERTO_MEMORIA");

	//int cliente_kernel = crear_conexion_al_server(logger, ip, puerto_memoria_kernel);

	// if (cliente_kernel)
	// {
	// 	log_info(logger, "El kernel envió su conexión a la memoria!");
	// }
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


	int conexion_consola = esperar_cliente(server_consola);
	log_info(logger, "El kernel recibió la conexión de consola");

//CREACION DE COLAS DE ESTADOS
	t_queue* cola_new = queue_create();
	t_queue* cola_ready = queue_create();
	t_queue* cola_blocked = queue_create();
	t_queue* cola_exit = queue_create();


	if (cliente_cpu)
	{	
		log_info(logger, "El kernel envió su conexión a la CPU!");
		char* codigo_recibido = recibir_mensaje(conexion_consola);
		log_info(logger, "Recibi de consola %s", codigo_recibido);
		t_pcb* pcb = crear_pcb(codigo_recibido);
		char* instruccion_serializada = list_get(pcb->contexto_de_ejecucion.lista_instrucciones, 0);
		log_info(logger, "La instruccion serializada es %s", instruccion_serializada);
		queue_push(cola_new, pcb);
		queue_push(cola_ready, queue_pop(cola_new));
		
		t_pcb* pcb_a_ejecutar = malloc(sizeof(t_pcb));
		pcb_a_ejecutar = queue_pop(cola_ready);
		enviar_contexto_de_ejecucion(&(pcb_a_ejecutar->contexto_de_ejecucion), cliente_cpu);


		t_contexto_de_ejecucion* contexto_actualizado = malloc(sizeof(t_contexto_de_ejecucion));
		contexto_actualizado = recibir_contexto_de_ejecucion(cliente_cpu);

		if (contexto_actualizado == NULL)
		{
			log_info(logger, "CPU no devolvió el contexto de ejecución");
			free(contexto_actualizado);
			EXIT_FAILURE;
		}

		//Tengo que switchear el código de respuesta que me mando la cpu en el contexto
		log_info(logger, "Recibi el contexto actualizado");

		//Guardo el contexto nuevo en el PCB de ese proceso
		pcb->contexto_de_ejecucion = *contexto_actualizado;
		log_info(logger, "En el contexto hay %i", contexto_actualizado->program_counter);
		log_info(logger, "El codigo de retorno es %i", contexto_actualizado->codigo_respuesta);
		
		int codigo_respuesta = pcb->contexto_de_ejecucion.codigo_respuesta;

		switch(codigo_respuesta){
			case 1: //YIELD: mando el proceso a ready de nuevo
				break;

			case 2: //EXIT: mando el proceso a la lista de exit
				break;
		}

		queue_push(cola_exit, pcb);
		liberar_pcb(queue_pop(cola_exit));
		liberar_contexto_de_ejecucion(contexto_actualizado);
				
	}

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

void enviar_instrucciones_a_cpu(void* arg) {
	parametros_hilo* params = (parametros_hilo*) arg;
	int conexion_consola = params->conexion_servidor;
	int cliente_cpu = params->conexion_cliente;
	char* codigo_recibido = recibir_mensaje(conexion_consola);
		t_pcb * pcb = crear_pcb(codigo_recibido);

		char * temp = list_get(pcb->contexto_de_ejecucion.lista_instrucciones, 0);
		printf("\n%s", temp);
		
		enviar_contexto_de_ejecucion(&(pcb->contexto_de_ejecucion), cliente_cpu);
}

/*void conectarse_con_consola(config) {
	char* puerto_kernel_consola = config_get_string_value(config, "PUERTO_CONSOLA");
	int server_consola = iniciar_servidor(logger, ip, puerto_kernel_consola);
	if (server_consola != -1)
	{
		log_info(logger, "El servidor del kernel se inició");
	}


	int conexion_consola = esperar_cliente(server_consola);
	log_info(logger, "El kernel recibió la conexión de consola");
}*/

// parametros_hilo params = {conexion_consola, cliente_cpu};
		// pthread_t hilo1;
		// pthread_create(&hilo1, NULL, (void*)enviar_instrucciones_a_cpu, (void*) &params);
		// log_info(logger, "El kernel envio el contexto de ejecucion al CPU!");
		//parametros_hilo* params = (parametros_hilo*) arg;
		//int conexion_consola = params->conexion_servidor;
		//int cliente_cpu = params->conexion_cliente;