#include "main.h"

/*------------------------------------------------------------------*/
/*|						CHECKPOINT 2					   			*/
/*------------------------------------------------------------------*/
/*		Levanta el archivo de configuración: HECHO
		Se conecta a Memoria y espera conexiones del Kernel: HECHO.
		Ejecuta las instrucciones SET, YIELD y EXIT.				*/
/*------------------------------------------------------------------*/
int conectarse_a_memoria(t_log* logger);

int main(void)
{
	t_log * logger = iniciar_logger("log_cpu.log","LOG_CPU");
	//t_config* config = iniciar_config("configs/cpu.config");
	
	//CPU como cliente para memoria
	//int conexion_memoria_cpu = conectarse_a_memoria(logger);


	//CPU como server del Kernel
	int conexion_cpu_kernel = conexion_a_kernel(logger);

	if(conexion_cpu_kernel)
	{
		log_info(logger, "CPU recibió al kernel");
	}
}

int conectarse_a_memoria(t_log* logger)
{
	t_config* config = config_create("/home/utnso/tp-2023-1c-BasadOS/BasadOS/cpu/configs/cpu.config");

	if (config == NULL)
	{
		log_info(logger, "Error abriendo el config");
		EXIT_FAILURE;
	}

	char* ip = config_get_string_value(config, "IP");
	char* puerto_memoria_cpu = config_get_string_value(config, "PUERTO_MEMORIA");

	int conexion_memoria_cpu = crear_conexion_al_server(logger, ip, puerto_memoria_cpu);

	if (conexion_memoria_cpu == -1)
	{
		log_info(logger, "Error conectandose a memoria");
	}
	
	return conexion_memoria_cpu;
}


int conexion_a_kernel(t_log* logger)
{
	t_config* config = config_create("/home/utnso/tp-2023-1c-BasadOS/BasadOS/cpu/configs/cpu.config");
	char* puerto_cpu_kernel =  config_get_string_value(config, "PUERTO_KERNEL");
	char* ip = config_get_string_value(config, "IP");

	int server_para_kernel = iniciar_servidor(logger, ip, puerto_cpu_kernel);

	int conexion_kernel = esperar_cliente(logger, server_para_kernel);


}