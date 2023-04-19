//#include "main.h"
#include "/home/utnso/tp-2023-1c-BasadOS/BasadOS/utils/src/sockets/socketUtils.c"
#include<commons/string.h>
#include<commons/config.h>

t_log * iniciar_logger(void);

int main(void)
{
	int conexion;
	char* ip = "127.0.0.1";
	char* puerto = "35823";

	t_log * logger = iniciar_logger();

	int cliente_kernel = crear_conexion_al_server(logger, ip, puerto);

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
/*
void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	 Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) 
	  con las funciones de las commons y del TP mencionadas en el enunciado 
	if(logger != NULL){
		log_destroy(logger);
	}
	if(config != NULL){
			config_destroy(config);
		}
	liberar_conexion(conexion);
}*/