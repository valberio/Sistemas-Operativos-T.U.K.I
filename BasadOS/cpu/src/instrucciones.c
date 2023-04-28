#include "registros.h"
#include <string.h>
#include<loggers/loggers_utils.h>


//Agrego una r a los nombres para que no se pisen con los nombres de los registros reales
enum Registros{
    rAX,
    rBX,
    rCX,
    rDX
};

enum Registros string_a_registro(char* registro);

void set(t_log* logger, char** instrucciones)
{
    //Guardo en el registro de registros.c el valor
    log_info(logger, "Entré en la ejecución de SET");
    
    enum Registros registro = string_a_registro(instrucciones[1]);
    char * valor = instrucciones[2];

    switch (registro){
        case rAX:
            AX = valor;
            log_info(logger, AX);
            break;
        case rBX:
            BX = valor;
            log_info(logger, BX);
            break;
        case rCX:
            CX = valor;
            log_info(logger, CX);
            break;
        case rDX:
            DX = valor;
            log_info(logger, DX);
            break;
        }    
}

void yield(t_log* logger)
{
    log_info(logger, "Entré en la ejecución de YIELD");
}

void exit_instruccion(t_log* logger)
{
    log_info(logger, "Entré en la ejecución de EXIT");
}


enum Registros string_a_registro(char* registro)
{
    if (strcmp(registro, "AX") == 0)
	{
		return rAX;
	}
	if (strcmp(registro, "BX") == 0)
	{
		return rBX;
	}
	if (strcmp(registro, "CX") == 0)
	{
		return rCX;
	}
    if (strcmp(registro, "DX") == 0)
	{
		return rDX;
	}
	return EXIT_FAILURE;
}
