#include "instrucciones.h"

// Agrego una r a los nombres para que no se pisen con los nombres de los registros reales

int set(t_log *logger, char **instrucciones, t_contexto_de_ejecucion *contexto)
{
    // Guardo en el registro de registros.c el valor

    log_info(logger, "PID: %i EJECUTANDO: %s PARAMETROS: %s, %s", contexto->pid, instrucciones[0], instrucciones[1], instrucciones[2]);

    enum Registros registro = string_a_registro(instrucciones[1]);
    char *valor = instrucciones[2];

    switch (registro)
    {
    case rAX:
        strcpy(contexto->registros->AX, valor);
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

    // Registros 8 bits
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
        log_info(logger, contexto->registros->EDX);
        break;

    // Registros de 16 bits
    case rRAX:
        strcpy(contexto->registros->RAX, valor);
        log_info(logger, contexto->registros->RAX);
        break;

    case rRBX:
        strcpy(contexto->registros->RBX, valor);
        log_info(logger, contexto->registros->RBX);
        break;

    case rRCX:
        strcpy(contexto->registros->RDX, valor);
        log_info(logger, contexto->registros->RDX);
        break;

    case rRDX:
        strcpy(contexto->registros->RDX, valor);
        log_info(logger, contexto->registros->RDX);
        break;
    }
    return 0;
}

int yield(t_log *logger, t_contexto_de_ejecucion *contexto, int conexion_cpu_kernel)
{
    // Tengo que armar el paquete con codigo de operacion 1
    log_info(logger, "PID: %i EJECUTANDO: YIELD PARAMETROS: -", contexto->pid);
    t_paquete *paquete = crear_paquete();
    paquete->codigo_operacion = INTERRUPCION_A_READY;
    paquete->buffer = serializar_contexto(contexto);

    enviar_paquete(paquete, conexion_cpu_kernel);
    eliminar_paquete(paquete);

    return 1;
}

int exit_instruccion(t_log *logger, t_contexto_de_ejecucion *contexto, int conexion_cpu_kernel)
{
    log_info(logger, "PID: %i EJECUTANDO: EXIT PARAMETROS: -", contexto->pid);
    // Tengo que armar el paquete con codigo de operacion 1
    t_paquete *paquete = crear_paquete();
    paquete->codigo_operacion = FINALIZACION;
    paquete->buffer = serializar_contexto(contexto);

    enviar_paquete(paquete, conexion_cpu_kernel);
    eliminar_paquete(paquete);

    return 1;
}

int wait(t_log *logger, t_contexto_de_ejecucion *contexto, int conexion_cpu_kernel, char **instrucciones)
{
    log_info(logger, "PID: %i EJECUTANDO: %s PARAMETROS: %s", contexto->pid, instrucciones[0], instrucciones[1]);
    t_paquete *paquete = crear_paquete();
    paquete->codigo_operacion = PETICION_RECURSO;
    paquete->buffer = serializar_contexto(contexto);

    enviar_paquete(paquete, conexion_cpu_kernel);
    eliminar_paquete(paquete);

    // Envio el parámetro
    enviar_mensaje(instrucciones[1], conexion_cpu_kernel);

    // Espero la respuesta de kernel sobre el recurso
    char *respuesta = recibir_mensaje(conexion_cpu_kernel);

    if (strcmp(respuesta, "0") == 0)
    {
        return 0;
    }

    return 1; // Si el recurso existe, sigue ejecutando; si no, desalojo
}

int signal_instruccion(t_log *logger, t_contexto_de_ejecucion *contexto, int conexion_cpu_kernel, char **instrucciones)
{
    log_info(logger, "PID: %i EJECUTANDO: %s PARAMETROS: %s", contexto->pid, instrucciones[0], instrucciones[1]);
    t_paquete *paquete = crear_paquete();
    paquete->codigo_operacion = LIBERACION_RECURSO;
    paquete->buffer = serializar_contexto(contexto);

    enviar_paquete(paquete, conexion_cpu_kernel);
    eliminar_paquete(paquete);

    // Envio el parámetro
    enviar_mensaje(instrucciones[1], conexion_cpu_kernel);

    // Espero la respuesta de kernel sobre el recurso
    char *respuesta = recibir_mensaje(conexion_cpu_kernel);

    if (strcmp(respuesta, "0") == 0)
    {
        return 0;
    }

    return 1; // Si el recurso existe, sigue ejecutando; si no, desalojo
}

int i_o(t_log *logger, t_contexto_de_ejecucion *contexto, int conexion_cpu_kernel, char **instrucciones)
{
    log_info(logger, "PID: %i EJECUTANDO: %s PARAMETROS: %s", contexto->pid, instrucciones[0], instrucciones[1]);
    t_paquete *paquete = crear_paquete();
    paquete->codigo_operacion = INTERRUPCION_BLOQUEANTE;
    paquete->buffer = serializar_contexto(contexto);

    enviar_paquete(paquete, conexion_cpu_kernel);
    eliminar_paquete(paquete);

    // Envio el parámetro
    enviar_mensaje(instrucciones[1], conexion_cpu_kernel);

    return 1;
}

// Esta funcion deberia estar en la carpeta del pcb
enum Registros string_a_registro(char *registro)
{
    if (strncmp(registro, "AX", 2) == 0)
    {
        return rAX;
    }
    if (strncmp(registro, "BX", 2) == 0)
    {
        return rBX;
    }
    if (strncmp(registro, "CX", 2) == 0)
    {
        return rCX;
    }
    if (strncmp(registro, "DX", 2) == 0)
    {
        return rDX;
    }

    // Registros de 8 bits
    if (strncmp(registro, "EAX", 3) == 0)
    {
        return rEAX;
    }
    if (strncmp(registro, "EBX", 3) == 0)
    {
        return rEBX;
    }
    if (strncmp(registro, "ECX", 3) == 0)
    {
        return rECX;
    }
    if (strncmp(registro, "EDX", 3) == 0)
    {
        return rEDX;
    }

    // Registros de 16 bits
    if (strncmp(registro, "RAX", 3) == 0)
    {
        return rRAX;
    }
    if (strncmp(registro, "REX", 3) == 0)
    {
        return rRBX;
    }
    if (strncmp(registro, "RCX", 3) == 0)
    {
        return rRCX;
    }
    if (strncmp(registro, "RDX", 3) == 0)
    {
        return rRDX;
    }
    return EXIT_FAILURE;
}

int mov_in(t_log *logger, char **instrucciones, t_contexto_de_ejecucion *contexto, int conexion_memoria_cpu)
{
    log_info(logger, "PID: %i EJECUTANDO: %s PARAMETROS: %s, %s", contexto->pid, instrucciones[0], instrucciones[1], instrucciones[2]);
    // Envio a memoria un paquete que indique con su código de operación que quiero LEER
    t_paquete *paquete = crear_paquete();
    paquete->codigo_operacion = PETICION_LECTURA; // Tengo que poder serializar la direccion de la que quiero leer

    enviar_paquete(paquete, conexion_memoria_cpu);

    // Espero la respuesta con el valor que se encontraba en la direccion
    t_paquete *paquete_respuesta = recibir_paquete(conexion_memoria_cpu);
    // Guardo ese valor en el registro correspondiente
    log_info(logger, "PID: %i EJECUTANDO: %s - PARAMETROS: %s, %s - RTA RECIBIDA: %i", contexto->pid, instrucciones[0], instrucciones[1], instrucciones[2], paquete_respuesta->codigo_operacion);

    return 0;
}

int mov_out(t_log *logger, char **instrucciones, t_contexto_de_ejecucion *contexto, int conexion_memoria_cpu)
{
    log_info(logger, "PID: %i EJECUTANDO: %s PARAMETROS: %s, %s", contexto->pid, instrucciones[0], instrucciones[1], instrucciones[2]);
    // Envio a memoria un paquete que indique con su código de operación que quiero ESCRIBIR
    t_paquete *paquete = crear_paquete();
    paquete->codigo_operacion = PETICION_ESCRITURA; // Tengo que poder serializar la direccion de la que quiero leer

    enviar_paquete(paquete, conexion_memoria_cpu);

    // Memoria responde con un OK confirmado que escribió lo que debía

    t_paquete *paquete_respuesta = recibir_paquete(conexion_memoria_cpu);

    log_info(logger, "PID: %i EJECUTANDO: %s - PARAMETROS: %s, %s - RTA RECIBIDA: %i", contexto->pid, instrucciones[0], instrucciones[1], instrucciones[2], paquete_respuesta->codigo_operacion);

    return 0;
}

int create_segment(t_log *logger, char **instrucciones, t_contexto_de_ejecucion *contexto, int conexion_kernel_cpu)
{
    // Le digo a KERNEL que le diga a MEMORIA que tiene que crear un segmento
    log_info(logger, "PID: %i EJECUTANDO: %s PARAMETROS: %s, %s", contexto->pid, instrucciones[0], instrucciones[1], instrucciones[2]);

    t_paquete *paquete = crear_paquete();
    paquete->codigo_operacion = CREAR_SEGMENTO;
    paquete->buffer = serializar_contexto(contexto);
    enviar_paquete(paquete, conexion_kernel_cpu);
    eliminar_paquete(paquete);

    return 1;
}

int delete_segment(t_log *logger, char **instrucciones, t_contexto_de_ejecucion *contexto, int conexion_kernel_cpu)
{
    // Le digo a KERNEL que le diga a MEMORIA que tiene que crear un segmento
    log_info(logger, "PID: %i EJECUTANDO: %s PARAMETROS: %s", contexto->pid, instrucciones[0], instrucciones[1]);

    t_paquete *paquete = crear_paquete();
    paquete->codigo_operacion = ELIMINAR_SEGMENTO;
    paquete->buffer = serializar_contexto(contexto);
    enviar_paquete(paquete, conexion_kernel_cpu);

    return 1;
}

int f_open(t_log *logger, t_contexto_de_ejecucion *contexto, int conexion_kernel_cpu, char **instrucciones)
{
    log_info(logger, "PID: %i EJECUTANDO: %s PARAMETROS: %s", contexto->pid, instrucciones[0], instrucciones[1]);
    t_paquete *paquete = crear_paquete();
    paquete->codigo_operacion = ABRIR_ARCHIVO;
    paquete->buffer = serializar_contexto(contexto);
    enviar_paquete(paquete, conexion_kernel_cpu);

    enviar_mensaje(instrucciones[1], conexion_kernel_cpu);

    char *respuesta = recibir_mensaje(conexion_kernel_cpu);
    if (strcmp(respuesta, "0") == 0)
    {
        return 0;
    }

    return 1; // Si el archivo esta en uso, corta la ejecucion
}


int f_close(t_log* logger, char** instrucciones, t_contexto_de_ejecucion* contexto, int conexion_kernel_cpu){
    log_info(logger, "PID: %i EJECUTANDO: %s PARAMETROS: %s", contexto->pid, instrucciones[0], instrucciones[1]);
    t_paquete* paquete = crear_paquete();
    paquete->codigo_operacion = CERRAR_ARCHIVO;
    paquete->buffer = serializar_contexto(contexto);
    enviar_paquete(paquete, conexion_kernel_cpu);
    enviar_mensaje(instrucciones[1], conexion_kernel_cpu);
    return 0;
}