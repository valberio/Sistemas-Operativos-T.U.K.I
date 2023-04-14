#include "client.h"


t_log* iniciar_logger(void)
{
	t_log* nuevo_logger;
	if((nuevo_logger = log_create("tp0.log", "PROCESS_TP0", 1, LOG_LEVEL_INFO)) == NULL){
		printf("No se pudo crear el logger.\n");
		exit(2);
	}

	return nuevo_logger;
}

t_config* iniciar_config(void)
{
	t_config* nuevo_config;
	if((nuevo_config = config_create("cliente.config")) == NULL){
		printf("No se pudo leer la config.\n");
		exit(2);
	}
	return nuevo_config;
}

void leer_consola(t_log* logger)
{
	char* leido;

	// La primera te la dejo de yapa
	//leido = readline("> ");

	// El resto, las vamos leyendo y logueando hasta recibir un string vacío

	while(1){
		leido = readline("> ");
		if(leido){
			//add_history(leido);
			log_info(logger, leido);
		}
		if(!strcmp(leido, "")){
			free(leido);
			break;
		}
	}

	// ¡No te olvides de liberar las lineas antes de regresar!
	//printf("%s\n", leido);
	//free(leido);
}

void paquete(int conexion)
{
	// Ahora toca lo divertido!
	char* leido;
	t_paquete* paquete;

	// Leemos y esta vez agregamos las lineas al paquete
	while(1){
			leido = readline("> ");
			paquete = crear_paquete();
			if(leido){
				add_history(leido);
				agregar_a_paquete(paquete, leido, (strlen(leido) + 1));
				enviar_paquete(paquete, conexion);
				eliminar_paquete(paquete);
			}
			if(!strcmp(leido, "")){
				free(leido);
				break;
			}
		}

	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!

}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) 
	  con las funciones de las commons y del TP mencionadas en el enunciado */
	if(logger != NULL){
		log_destroy(logger);
	}
	if(config != NULL){
			config_destroy(config);
		}
	liberar_conexion(conexion);
}
