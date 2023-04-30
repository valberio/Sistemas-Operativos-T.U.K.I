#include <main.h>


/*------------------------------------------------------------------*/
/*						CHECKPOINT 2								*/
/*------------------------------------------------------------------*/
/*		Levanta el archivo de configuración: HECHO!
		Se conecta a Memoria y espera la conexión de Kernel: HECHO!
																	*/
/*------------------------------------------------------------------*/					



int main(void)
{
	t_log * logger = iniciar_logger("log_filesystem.log", "LOG_FILESYSTEM");
	t_config* config = iniciar_config("../configs/filesystem.config");

	char* ip = config_get_string_value(config, "IP");

	//Conecto filesystem como cliente a memoria
	char* puerto_a_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
	int cliente_filesystem_a_memoria = crear_conexion_al_server(logger, ip, puerto_a_memoria);
	if (cliente_filesystem_a_memoria)
	{
		log_info(logger, "Filesystem se conectó a memoria!");
	}


	//Conecto el filesystem como servidor del kernel
	char* puerto_a_kernel = config_get_string_value(config, "PUERTO_KERNEL");
	int servidor_filesystem = iniciar_servidor(logger, ip, puerto_a_kernel);
	int conexion_filesystem_kernel = esperar_cliente(logger, servidor_filesystem);

	if (conexion_filesystem_kernel)
	{
		log_info(logger, "Filesystem recibió la conexión del kernel!");
	}
}
