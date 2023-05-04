#include "main_cpu.h"



/*------------------------------------------------------------------*/
/*|						CHECKPOINT 2					   			*/
/*------------------------------------------------------------------*/
/*		Levanta el archivo de configuración: HECHO
		Se conecta a Memoria y espera conexiones del Kernel: HECHO.
		Ejecuta las instrucciones SET, YIELD y EXIT.				*/
/*------------------------------------------------------------------*/


//Posibles mejoras:
//	-Que en vez de un array de string que switcheo a sus enums
//	 correspondientes, decode() me devuelva una lista de enums -> descartado, no todas las instrucciones tienen los mismos enums como parametros

//Pasar TODO a strings y usar las funciones de strings de la cátedra

//Debatir: que condicion de corte le pongo al while?

enum Instrucciones{
	SET,
	YIELD,
	EXIT
};


enum Instrucciones string_a_instruccion(char* instruccion);

int main(void)
{
	t_log * logger = iniciar_logger("log_cpu.log","LOG_CPU");
	
	t_config* config = iniciar_config("../configs/cpu.config");
	
	//CPU como cliente para memoria
	//int conexion_memoria_cpu = conectarse_a_memoria(logger);


	//CPU como server del Kernel
	int conexion_cpu_kernel = conexion_a_kernel(config, logger);

	if(conexion_cpu_kernel)
	{
		log_info(logger, "CPU recibió al kernel");
		t_contexto_de_ejecucion* contexto = recibir_contexto_de_ejecucion(conexion_cpu_kernel);

		int cant_instrucciones = list_size(contexto->lista_instrucciones);

		while (contexto->program_counter <= cant_instrucciones)
		{
			char * instruccion = fetch(contexto);

			char ** instruccion_array = decode(instruccion);

			execute(logger, instruccion_array, &(contexto->registros));

			contexto->program_counter++;
		}
	}
	return 0;
}

int conectarse_a_memoria(t_config* config, t_log* logger)
{
	char* ip = config_get_string_value(config, "IP");
	char* puerto_memoria_cpu = config_get_string_value(config, "PUERTO_MEMORIA");

	int conexion_memoria_cpu = crear_conexion_al_server(logger, ip, puerto_memoria_cpu);

	if (conexion_memoria_cpu == -1)
	{
		log_info(logger, "Error conectandose a memoria");
	}
	
	return conexion_memoria_cpu;
}


int conexion_a_kernel(t_config* config,t_log* logger)
{
	char* puerto_cpu_kernel =  config_get_string_value(config, "PUERTO_KERNEL");
	char* ip = config_get_string_value(config, "IP");

	int server_para_kernel = iniciar_servidor(logger, ip, puerto_cpu_kernel);

	int conexion_kernel = esperar_cliente(server_para_kernel);
	return conexion_kernel;

}

char * fetch(t_contexto_de_ejecucion * contexto)
{
	int program_counter = contexto->program_counter;

	return list_get(contexto->lista_instrucciones, program_counter);
}

char** decode(char* instruccion)
{
	//Todavia no hay que hacer traducciones de memoria, asi que retorna
	//el string de la instruccion en un array de chars, para facilitar
	//el acceso a los datos en execute()
	return string_split(instruccion, " ");
}

enum Instrucciones string_a_instruccion(char* instruccion)
{

	if (strcmp(instruccion, "SET") == 0)
	{
		return SET;
	}
	if (strcmp(instruccion, "YIELD") == 0)
	{
		return YIELD;
	}
	if (strcmp(instruccion, "EXIT") == 0)
	{
		return EXIT;
	}
	return EXIT_FAILURE;
}



void execute(t_log* logger, char** instrucciones, t_registros * registros)
{
	//Switcheo sobre el primer elemento del array de instrucciones
	enum Instrucciones instruccion = string_a_instruccion(instrucciones[0]);

	switch(instruccion) {
		case SET:
			set(logger, instrucciones, registros);
			break;
		case YIELD:
			yield(logger);
			break;
		case EXIT:
			exit_instruccion(logger);
			break;
		default:
			EXIT_FAILURE;
	}
	
}

