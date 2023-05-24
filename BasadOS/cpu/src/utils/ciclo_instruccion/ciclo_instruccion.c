#include "ciclo_instruccion.h"

char * fetch(t_contexto_de_ejecucion * contexto)
{
	int program_counter = contexto->program_counter;

	return list_get(contexto->instrucciones, program_counter);
}

char** decode(char* instruccion)
{
	//Todavia no hay que hacer traducciones de memoria, asi que retorna
	//el string de la instruccion en un array de chars, para facilitar
	//el acceso a los datos en execute()
	char ** array = string_split(instruccion, " ");
	return array;
}

int execute(t_log* logger, char** instrucciones, t_contexto_de_ejecucion* contexto, int conexion_cpu_kernel)
{
	//Switcheo sobre el primer elemento del array de instrucciones
	enum Instrucciones instruccion = string_a_instruccion(instrucciones[0]);
	int resultado;
	switch(instruccion) {
		case SET:
			resultado = set(logger, instrucciones, contexto);
			break;
		case YIELD:
			resultado = yield(logger, contexto, conexion_cpu_kernel);
			break;
		case EXIT:
			resultado = exit_instruccion(logger, contexto, conexion_cpu_kernel);
			break;
		case I_O:
			resultado = i_o(logger, contexto, conexion_cpu_kernel, instrucciones);
			break;
		case WAIT:
			resultado = wait(logger, contexto, conexion_cpu_kernel, instrucciones);
			break;
		case SIGNAL:
			resultado = signal_instruccion(logger, contexto, conexion_cpu_kernel, instrucciones);
			break;
		default:
			break;
	}
	return resultado;
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
	if (strcmp(instruccion, "I/O") == 0)
	{
		return I_O;
	}
	if (strcmp(instruccion, "WAIT") == 0)
	{
		return WAIT;
	}
	if (strcmp(instruccion, "SIGNAL") == 0)
	{
		return SIGNAL;
	}
	return EXIT_FAILURE;
}

void liberar_array_instrucciones(char** array_instrucciones)
{
    char** temp = array_instrucciones;
    while (*temp) {
        free(*temp);
        temp++;
    }
    free(array_instrucciones);
}