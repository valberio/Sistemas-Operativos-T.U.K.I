#include "main.h"

int main(void)
{
	char* ip;
	char* puerto;
	
	t_log * logger = iniciar_logger();
	t_config* config = iniciar_config();
	
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");
	printf("%s, %s", ip, puerto);
	int cliente_kernel = crear_conexion_al_server(logger, ip, puerto);
	

	//Abro el server del kernel para recibir conexiones de la consola
	char* puerto_consola = "10577"; // se tiene que ir al config
	int server_consola = iniciar_servidor(logger, ip, puerto_consola);
	
	if (server_consola != -1)
	{
		log_info(logger, "El servidor del kernel se inició");
	}

	int conexion_consola = esperar_cliente(logger, server_consola);
	if (conexion_consola)
	{
		log_info(logger, "El kernel recibió la conexión de consola");
	}

	//Conecto el kernel como cliente a la CPU
	char* puerto_cpu_kernel =  "34343";
	int cliente_cpu = crear_conexion_al_server(logger, ip, puerto_cpu_kernel);
	if (cliente_cpu)
	{
		log_info(logger, "El kernel envió su conexión a la CPU!");
	}


	//Conecto el kernel como cliente del filesystem
	char* puerto_filesystem = "37373";
	int cliente_filesystem = crear_conexion_al_server(logger, ip, puerto_filesystem);
	if (cliente_filesystem)
	{
		log_info(logger, "El kernel envió su conexión al filesystem!");
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
	nuevo_config = config_create("../configs/config_kernel.config");
	if(nuevo_config == NULL){
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