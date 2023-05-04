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


int main(void)
{
	char* ip;
	
	t_log* logger = iniciar_logger("log_kernel.log", "LOG_KERNEL");
	t_config* config = iniciar_config("../configs/config_kernel.config");
	

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
	 if (cliente_cpu)
	 {
		
	 	log_info(logger, "El kernel envió su conexión a la CPU!");
	 }

	// //Conecto el kernel como cliente del filesystem
	// char* puerto_filesystem_kernel = config_get_string_value(config, "PUERTO_FILESYSTEM");;
	// //int cliente_filesystem = crear_conexion_al_server(logger, ip, puerto_filesystem_kernel);
	// if (cliente_filesystem)
	// {
	// 	log_info(logger, "El kernel envió su conexión al filesystem!");
	// }
	
	//Abro el server del kernel para recibir conexiones de la consola
	char* puerto_kernel_consola = config_get_string_value(config, "PUERTO_CONSOLA");
	int server_consola = iniciar_servidor(logger, ip, puerto_kernel_consola);
	if (server_consola != -1)
	{
		log_info(logger, "El servidor del kernel se inició");
	}


	int conexion_consola = esperar_cliente(server_consola);


	if (conexion_consola)
	{	
		log_info(logger, "El kernel recibió la conexión de consola");
		char* codigo_recibido = recibir_mensaje(conexion_consola);
		t_pcb * pcb = crear_pcb(codigo_recibido);

		char * temp = list_get(pcb->contexto_de_ejecucion.lista_instrucciones, 0);
		printf("\n%s", temp);
		
		enviar_contexto_de_ejecucion(&(pcb->contexto_de_ejecucion),cliente_cpu);
		log_info(logger, "El kernel envio el contexto de ejecucion al CPU!");
		//liberar_pcb(pcb);
		
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

void iterator(char* value) {
	printf("%s", value);
	//log_info(logger,"%s", value);
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
