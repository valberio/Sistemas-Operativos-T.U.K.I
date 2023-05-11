#include "instrucciones.h"

//Agrego una r a los nombres para que no se pisen con los nombres de los registros reales



void set(t_log* logger, char** instrucciones, t_registros* registros)
{
    //Guardo en el registro de registros.c el valor
    log_info(logger, "Entré en la ejecución de SET");
    
    enum Registros registro = string_a_registro(instrucciones[1]);
    char * valor = instrucciones[2];

    switch (registro){
        case rAX:
            strcpy(registros->AX, valor);
            //log_info(logger, registros->AX);
            break;
        case rBX:
            strcpy(registros->BX, valor);
            //log_info(logger, registros->BX);
            break;
        case rCX:
            strcpy(registros->CX, valor);
            //log_info(logger, registros->CX);
            break;
        case rDX:
            strcpy(registros->DX, valor);
            //log_info(logger, registros->DX);
            break;
    
		//Registros 8 bits
		case rEAX:
            strcpy(registros->EAX, valor);
            //log_info(logger, registros->EAX);
            break;
        
		case rEBX:
            strcpy(registros->EBX, valor);
            //log_info(logger, registros->EBX);
            break;
        
		case rECX:
            strcpy(registros->EDX, valor);
            //log_info(logger, registros->EDX);
            break;
        
		case rEDX:
            strcpy(registros->EDX, valor);
            //log_info(logger, registros->EDX);
            break;
        
		//Registros de 16 bits
		case rRAX:
            strcpy(registros->RAX, valor);
            //log_info(logger, registros->RAX);
            break;
        
		case rRBX:
            strcpy(registros->RBX, valor);
            //log_info(logger, registros->RBX);
            break;
        
		case rRCX:
            strcpy(registros->RDX, valor);
            //log_info(logger, registros->RDX);
            break;
        
		case rRDX:
            strcpy(registros->RDX, valor);
            //log_info(logger, registros->RDX);
            break;
		}
}

void yield(t_log* logger)
{
    //Actualizo el estado del proceso a READY
    log_info(logger, "Entré en la ejecución de YIELD");
}

void exit_instruccion(t_log* logger)
{
    //Actualizo el estado del proceso a EXIT
    log_info(logger, "Entré en la ejecución de EXIT");
}


//Esta funcion deberia estar en la carpeta del pcb
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

    //Registros de 8 bits
    if (strcmp(registro, "EAX") == 0)
	{
		return rEAX;
	}
    if (strcmp(registro, "EBX") == 0)
	{
		return rEBX;
	}
    if (strcmp(registro, "ECX") == 0)
	{
		return rECX;
	}
    if (strcmp(registro, "EDX") == 0)
	{
		return rEDX;
	}

    //Registros de 16 bits
    if (strcmp(registro, "RAX") == 0)
	{
		return rRAX;
	}
    if (strcmp(registro, "REX") == 0)
	{
		return rRBX;
	}
    if (strcmp(registro, "RCX") == 0)
	{
		return rRCX;
	}
    if (strcmp(registro, "RDX") == 0)
	{
		return rRDX;
	}
	return EXIT_FAILURE;
}
