#include "main.h"


t_log* iniciar_logger(void);

int main(void)
{
	int conexion;
	char* ip = "127.0.0.1";
	char* puerto = "35789";

	t_log * logger = iniciar_logger();

	int cliente_cpu = crear_conexion_al_server(logger, ip, puerto);

	//CPU como server del Kernel
	char* puerto_kernel =  "34343";
	int server_para_kernel = iniciar_servidor(logger, ip, puerto_kernel);

	int conexion_kernel = esperar_cliente(logger, server_para_kernel);

	if (conexion_kernel)
	{
		log_info(logger, "El kernel se conectó a la CPU!");
	}
	

}


t_log * iniciar_logger(void)
{
	t_log * nuevo_logger;
	nuevo_logger = log_create("log_kernel.log", "LOG_KERNEL", 1, LOG_LEVEL_INFO);
	return nuevo_logger;
}

t_config* iniciar_config(void)
{
	t_config* nuevo_config;
	if((nuevo_config = config_create("configs/config_kernel.config")) == NULL){
		printf("No se pudo leer la config.\n");
		exit(2);
	}
	return nuevo_config;
}
