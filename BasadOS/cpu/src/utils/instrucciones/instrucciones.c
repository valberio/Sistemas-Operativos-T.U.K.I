#include "instrucciones.h"

//Agrego una r a los nombres para que no se pisen con los nombres de los registros reales



void set(t_log* logger, char* instrucciones, t_contexto_de_ejecucion* contexto) {
    //Guardo en el registro de registros.c el valor
    log_info(logger, "Entré en la ejecución de SET");
    
    enum Registros registro = string_a_registro(instrucciones[1]);
    char * valor = instrucciones[2];

    switch (registro){
        case rAX:
            strcpy(contexto->registros->AX , valor);
            log_info(logger, contexto->registros->AX);
            break;
        case rBX:
            strcpy(contexto->registros->BX, valor);
            log_info(logger, contexto->registros->BX);
            break;
        case rCX:
            strcpy(contexto->registros->CX, valor);
            log_info(logger, contexto->registros->CX);
            break;
        case rDX:
            strcpy(contexto->registros->DX, valor);
            log_info(logger, contexto->registros->DX);
            break;
    
		//Registros 8 bits
		case rEAX:
            strcpy(contexto->registros->EAX, valor);
            log_info(logger, contexto->registros->EAX);
            break;
    
		case rEBX:
            strcpy(contexto->registros->EBX, valor);
            log_info(logger, contexto->registros->EBX);
            break;
        
		case rECX:
            strcpy(contexto->registros->EDX, valor);
            log_info(logger, contexto->registros->EDX);
            break;
        
		case rEDX:
            strcpy(contexto->registros->EDX, valor);
            //log_info(logger, registros->EDX);
            break;
        
		//Registros de 16 bits
		case rRAX:
            strcpy(contexto->registros->RAX, valor);
            //log_info(logger, registros->RAX);
            break;
        
		case rRBX:
            strcpy(contexto->registros->RBX, valor);
            //log_info(logger, registros->RBX);
            break;
        
		case rRCX:
            strcpy(contexto->registros->RDX, valor);
            //log_info(logger, registros->RDX);
            break;
        
		case rRDX:
            strcpy(contexto->registros->RDX, valor);
            //log_info(logger, registros->RDX);
            break;
		}
}

void yield(t_log* logger, t_contexto_de_ejecucion* contexto, int conexion_cpu_kernel) {
    //Tengo que armar el paquete con codigo de operacion 1
    log_info(logger, "Entré en la ejecución de YIELD");
    t_paquete* paquete = crear_paquete();
    paquete->codigo_operacion = MENSAJE;
    paquete->buffer = serializar_contexto(contexto);

    enviar_paquete(paquete, conexion_cpu_kernel);
}

void exit_instruccion(t_log* logger, t_contexto_de_ejecucion* contexto, int conexion_cpu_kernel) {
    log_info(logger, "Entré en la ejecución de EXIT");
    //Tengo que armar el paquete con codigo de operacion 1
    t_paquete* paquete = crear_paquete();
    paquete->codigo_operacion = PAQUETE;
    paquete->buffer = serializar_contexto(contexto);

    enviar_paquete(paquete, conexion_cpu_kernel);
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
