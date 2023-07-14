#include "ciclo_instruccion.h"

char *fetch(t_contexto_de_ejecucion *contexto)
{
	int program_counter = contexto->program_counter;

	return list_get(contexto->instrucciones, program_counter);
}

char **decode(char *instruccion, int retardo_instruccion, int tam_max_segmento, t_log *logger, t_contexto_de_ejecucion *contexto) // Cambiar a SWITCH
{
	// Todavia no hay que hacer traducciones de memoria, asi que retorna
	// el string de la instruccion en un array de chars, para facilitar
	// el acceso a los datos en execute()
	char *separador = " ";
	char **array = string_split(instruccion, separador);

	int dir_fisica_int = 0;
	int dir_logica_int;
	int bytes_a_escribir = 0;

	if (strcmp(array[0], "SET") == 0)
	{
		log_info(logger, "Entre en la ejecucion de DECODE- SET con %i segundos de retraso de instruccion\n", retardo_instruccion);
		sleep(retardo_instruccion);
	}
	if (strcmp(array[0], "MOV_IN") == 0)
	{
		dir_logica_int = atoi(array[2]);
		bytes_a_escribir = tamanio_del_registro(array[1]);
		dir_fisica_int = traduccion_dir_logica_fisica(dir_logica_int, contexto->tabla_segmentos, tam_max_segmento, bytes_a_escribir);
		char *dir_fisica_string = int_a_string(dir_fisica_int);
		log_info(logger, "Entré en DECODE - MOV IN -> Traduccion de direccion lógica %i a física %s", dir_logica_int, dir_fisica_string);
		strcpy(array[2], dir_fisica_string);
	}
	if (strcmp(array[0], "MOV_OUT") == 0)
	{

		dir_logica_int = atoi(array[1]);
		bytes_a_escribir = tamanio_del_registro(array[2]);
		dir_fisica_int = traduccion_dir_logica_fisica(dir_logica_int, contexto->tabla_segmentos, tam_max_segmento, bytes_a_escribir);
		char *dir_fisica_string = int_a_string(dir_fisica_int);
		log_info(logger, "Entré en DECODE - MOV OUT -> Traduccion de direccion lógica %i a física %s", dir_logica_int, dir_fisica_string);
		strcpy(array[1], dir_fisica_string);
	}
	if (strcmp(array[0], "F_READ") == 0)
	{
		dir_logica_int = atoi(array[2]);
		bytes_a_escribir = atoi(array[3]);
		dir_fisica_int = traduccion_dir_logica_fisica(dir_logica_int, contexto->tabla_segmentos, tam_max_segmento, bytes_a_escribir);
		char *dir_fisica_string = int_a_string(dir_fisica_int);
		log_info(logger, "Entré en DECODE - F_READ -> Traduccion de direccion lógica %i a física %s", dir_logica_int, dir_fisica_string);
		strcpy(array[2], dir_fisica_string);
	}
	if (strcmp(array[0], "F_WRITE") == 0)
	{
		dir_logica_int = atoi(array[2]);
		bytes_a_escribir = atoi(array[3]);
		dir_fisica_int = traduccion_dir_logica_fisica(dir_logica_int, contexto->tabla_segmentos, tam_max_segmento, bytes_a_escribir);
		char *dir_fisica_string = int_a_string(dir_fisica_int);
		log_info(logger, "Entré en DECODE - F_WRITE -> Traduccion de direccion lógica %i a física %s", dir_logica_int, dir_fisica_string);
		strcpy(array[2], dir_fisica_string);
	}

	if (dir_fisica_int == -1)
	{
		array[0] = malloc(strlen("SEGFAULT") + 1);
		strcpy(array[0], "SEGFAULT");
		log_info(logger, "Error Segmentation Fault: “PID: %i - Error SEG_FAULT- Segmento: %i - Offset: %i - Tamaño: %i",
				 contexto->pid, dir_logica_int, (dir_logica_int % tam_max_segmento), bytes_a_escribir);
	}

	return array;
}

int execute(t_log *logger, char **instrucciones, t_contexto_de_ejecucion *contexto, int conexion_cpu_kernel, int conexion_memoria_cpu)
{
	// Switcheo sobre el primer elemento del array de instrucciones
	enum Instrucciones instruccion = string_a_instruccion(instrucciones[0]);
	int resultado;
	switch (instruccion)
	{
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
		resultado = f_open(logger, contexto, conexion_cpu_kernel, instrucciones);
		break;
	case F_CLOSE:
		resultado = f_close(logger, instrucciones, contexto, conexion_cpu_kernel);
		break;
	case F_SEEK:
		resultado = f_seek(logger, instrucciones, contexto, conexion_cpu_kernel);
		break;
	case F_READ:
		resultado = f_read(logger, instrucciones, contexto, conexion_cpu_kernel);
		break;
	case F_WRITE:
		resultado = f_write(logger, instrucciones, contexto, conexion_cpu_kernel);
		break;
	case F_TRUNCATE:
		resultado = f_truncate(logger, instrucciones, contexto, conexion_cpu_kernel);
		break;
	case WAIT:
		resultado = wait(logger, contexto, conexion_cpu_kernel, instrucciones);
		break;
	case SIGNAL:
		resultado = signal_instruccion(logger, contexto, conexion_cpu_kernel, instrucciones);
		break;
	case CREATE_SEGMENT:
		resultado = create_segment(logger, instrucciones, contexto, conexion_cpu_kernel);
		break;
	case DELETE_SEGMENT:
		resultado = delete_segment(logger, instrucciones, contexto, conexion_cpu_kernel);
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

enum Instrucciones string_a_instruccion(char *instruccion)
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

void liberar_array_instrucciones(char **array_instrucciones)
{
	char **temp = array_instrucciones;
	while (*temp)
	{
		free(*temp);
		temp++;
	}
	free(array_instrucciones);
}
