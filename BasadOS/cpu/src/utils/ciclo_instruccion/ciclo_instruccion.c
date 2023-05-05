#include "ciclo_instruccion.h"

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

int execute(t_log* logger, char** instrucciones, t_registros * registros)
{
	//Switcheo sobre el primer elemento del array de instrucciones
	enum Instrucciones instruccion = string_a_instruccion(instrucciones[0]);

	switch(instruccion) {
		case SET:
			set(logger, instrucciones, registros);
			return 0;
			break;
		case YIELD:
			yield(logger);
			return 1;
			break;
		case EXIT:
			exit_instruccion(logger);
			return 1;
			break;
		default:
			return 0;
	}
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