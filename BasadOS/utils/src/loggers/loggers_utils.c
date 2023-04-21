#include "loggers_utils.h"

t_log * iniciar_logger(char* archivo, char* nombre_del_proceso)
{
	t_log* nuevo_logger;
	nuevo_logger = log_create(archivo, nombre_del_proceso, 1, LOG_LEVEL_INFO);
	return nuevo_logger;
}

t_config* iniciar_config(char* path)
{
	t_config* nuevo_config;
	nuevo_config = config_create(path);
	if(nuevo_config == NULL){
		printf("No se pudo leer la config.\n");
		exit(1);
	}
	return nuevo_config;
}
