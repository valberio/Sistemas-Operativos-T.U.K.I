#include "client.h"

int main(void)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	int conexion;
	char* ip;
	char* puerto;
	char* valor;

	t_log* logger;
	t_config* config;

	/* ---------------- LOGGING ---------------- */

	logger = iniciar_logger();

	// Usando el logger creado previamente
	// Escribi: "Hola! Soy un log"
	log_info(logger, "Soy un Log");


	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config();
	valor = config_get_string_value(config, "CLAVE");
	log_info(logger, valor);
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");

	// Usando el config creado previamente, leemos los valores del config y los 
	// dejamos en las variables 'ip', 'puerto' y 'valor'

	// Loggeamos el valor de config


	/* ---------------- LEER DE CONSOLA ---------------- */


	//leer_consola(logger);

	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él
	//printf("IP: %s - PUERTO: %s\n", ip, puerto);
	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(ip, puerto);

	// Enviamos al servidor el valor de CLAVE como mensaje
	enviar_mensaje(valor, conexion);
	// Armamos y enviamos el paquete
	paquete(conexion);
	terminar_programa(conexion, logger, config);

	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
	// Proximamente
}

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
			add_history(leido);
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
