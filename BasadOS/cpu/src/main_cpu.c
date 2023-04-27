#include "main.h"

/*------------------------------------------------------------------*/
/*|						CHECKPOINT 2					   			*/
/*------------------------------------------------------------------*/
/*		Levanta el archivo de configuración: HECHO
		Se conecta a Memoria y espera conexiones del Kernel: HECHO.
		Ejecuta las instrucciones SET, YIELD y EXIT.				*/
/*------------------------------------------------------------------*/

int main(void)
{
	t_log * logger = iniciar_logger("log_cpu.log","LOG_CPU");
	t_config* config = iniciar_config("configs/cpu.config");
	
	char* ip = config_get_string_value(config, "IP");
	
	//CPU como cliente para memoria
	char* puerto_memoria_cpu = config_get_string_value(config, "PUERTO_MEMORIA");
	int cliente_cpu = crear_conexion_al_server(logger, ip, puerto_memoria_cpu);

	//CPU como server del Kernel
	char* puerto_cpu_kernel =  config_get_string_value(config, "PUERTO_KERNEL");
	int server_para_kernel = iniciar_servidor(logger, ip, puerto_cpu_kernel);

	int conexion_kernel = esperar_cliente(logger, server_para_kernel);

	if (conexion_kernel)
	{
		log_info(logger, "El kernel se conectó a la CPU!");
	}
	
}


