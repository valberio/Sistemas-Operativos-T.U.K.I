#include "ciclo_instruccion.h"

char * fetch(t_contexto_de_ejecucion * contexto)
{
	int program_counter = contexto->program_counter;

	return list_get(contexto->instrucciones, program_counter);
}

char** decode(char* instruccion, int retardo_instruccion)
{
	//Todavia no hay que hacer traducciones de memoria, asi que retorna
	//el string de la instruccion en un array de chars, para facilitar
	//el acceso a los datos en execute()
	char ** array = string_split(instruccion, " ");

	if (strcmp(array[0], "SET") == 0)
	{
		printf("Entre en la ejecucion de DECODE- SET con %i segundos de retraso de instruccion\n", retardo_instruccion);
		sleep(retardo_instruccion);
	}

	return array;
}

int execute(t_log* logger, char** instrucciones, t_contexto_de_ejecucion* contexto, int conexion_cpu_kernel, int conexion_memoria_cpu)
{
	//Switcheo sobre el primer elemento del array de instrucciones
	enum Instrucciones instruccion = string_a_instruccion(instrucciones[0]);
	int resultado;
	switch(instruccion) {
		case SET:
			resultado = set(logger, instrucciones, contexto);
			break;
		case MOV_IN:
			resultado = mov_in(logger, instrucciones, contexto, conexion_memoria_cpu);
			break;
		case MOV_OUT:
			resultado = mov_out(logger, instrucciones, contexto, conexion_memoria_cpu);
			break;
		case I_O:
			resultado = i_o(logger, contexto, conexion_cpu_kernel, instrucciones);
			break;
		case F_OPEN:
			break;
		case F_CLOSE:
			break;
		case F_SEEK:
			break;
		case F_READ:
			break;
		case F_WRITE:
			break;
		case F_TRUNCATE:
			break;
		case WAIT:
			resultado = wait(logger, contexto, conexion_cpu_kernel, instrucciones);
			break;
		case SIGNAL:
			resultado = signal_instruccion(logger, contexto, conexion_cpu_kernel, instrucciones);
			break;
		case CREATE_SEGMENT:
			break;
		case DELETE_SEGMENT:
			break;			
		case YIELD:
			resultado = yield(logger, contexto, conexion_cpu_kernel);
			break;
		case EXIT:
			resultado = exit_instruccion(logger, contexto, conexion_cpu_kernel);
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
	if (strcmp(instruccion, "MOV_IN") == 0)
	{
		return MOV_IN;
	}
	if (strcmp(instruccion, "MOV_OUT") == 0)
	{
		return MOV_OUT;
	}
	if (strcmp(instruccion, "I/O") == 0)
	{
		return I_O;
	}
	if (strcmp(instruccion, "F_OPEN") == 0)
	{
		return F_OPEN;
	}
	if (strcmp(instruccion, "F_CLOSE") == 0)
	{
		return F_CLOSE;
	}
	if (strcmp(instruccion, "F_SEEK") == 0)
	{
		return F_SEEK;
	}
	if (strcmp(instruccion, "F_READ") == 0)
	{
		return F_READ;
	}
	if (strcmp(instruccion, "F_WRITE") == 0)
	{
		return F_WRITE;
	}
	if (strcmp(instruccion, "F_TRUNCATE") == 0)
	{
		return F_TRUNCATE;
	}
	if (strcmp(instruccion, "WAIT") == 0)
	{
		return WAIT;
	}

	if (strcmp(instruccion, "SIGNAL") == 0)
	{
		return SIGNAL;
	}
	if (strcmp(instruccion, "CREATE_SEGMENT") == 0)
	{
		return CREATE_SEGMENT;
	}
	if (strcmp(instruccion, "DELETE_SEGMENT") == 0)
	{
		return DELETE_SEGMENT;
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

void liberar_array_instrucciones(char** array_instrucciones)
{
    char** temp = array_instrucciones;
    while (*temp) {
        free(*temp);
        temp++;
    }
    free(array_instrucciones);
}