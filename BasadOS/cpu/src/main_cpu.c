#include "main.h"



/*------------------------------------------------------------------*/
/*|						CHECKPOINT 2					   			*/
/*------------------------------------------------------------------*/
/*		Levanta el archivo de configuración: HECHO
		Se conecta a Memoria y espera conexiones del Kernel: HECHO.
		Ejecuta las instrucciones SET, YIELD y EXIT.				*/
/*------------------------------------------------------------------*/


//Posibles mejoras:
//	-Que en vez de un array de string que switcheo a sus enums
//	 correspondientes, decode() me devuelva una lista de enums

//Pasar TODO a strings y usar las funciones de strings de la cátedra

enum Instrucciones{
	SET,
	YIELD,
	EXIT
};

enum Instrucciones string_a_instruccion(char* instruccion);

int main(void)
{
	t_log * logger = iniciar_logger("log_cpu.log","LOG_CPU");
	
	//t_config* config = iniciar_config("configs/cpu.config");
	
	//CPU como cliente para memoria
	//int conexion_memoria_cpu = conectarse_a_memoria(logger);


	//CPU como server del Kernel
	/*int conexion_cpu_kernel = conexion_a_kernel(logger);

	if(conexion_cpu_kernel)
	{
		log_info(logger, "CPU recibió al kernel");
	}
*/

	//FETCH: Busco la instrucción siguiente que me mandó el kernel
	char* instruccion_string = fetch();

	//DECODE: Decodifico la instruccion. Paso el string a un array,
	//determino qué instrucción del enum de instrucciones tengo que
	//ejecutar.


	char** instruccion_array = string_split(instruccion_string, " ");

	execute(logger, instruccion_array);

	/*if (string_a_instruccion("SET"))
	{
		log_info(logger, "p");
	}
	else {log_info(logger, "nope");}*/

	return 0;
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
	return conexion_kernel;

}

char* fetch(void)
{
	return "SET AX AB";
}

enum Instrucciones string_a_instruccion(char* instruccion)
{

	if (strcmp(instruccion, "SET") == 0)
	{
		return SET;
	}
	if (strcmp(instruccion, "YIELD") == 0)
	{
		return true;
	}
	if (strcmp(instruccion, "EXIT") == 0)
	{
		return true;
	}
	return EXIT_FAILURE;
}


void execute(t_log* logger, char** instrucciones)
{
	//Switcheo sobre el primer elemento del array de instrucciones
	enum Instrucciones instruccion = string_a_instruccion(instrucciones[0]);

	switch(instruccion) {
		case SET:
			set(logger, instrucciones);
			log_info(logger, AX);
			break;
		case YIELD:
			yield(logger, instrucciones);
			break;
		case EXIT:
			exit_instruccion(logger, instrucciones);
			break;
		default:
			EXIT_FAILURE;
	}

}

