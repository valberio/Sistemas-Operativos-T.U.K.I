#include "conexiones_memoria.h"

void* comunicacion_con_kernel(void* arg)
{
    parametros_de_hilo* parametros = (parametros_de_hilo*)arg;

    int conexion_kernel = parametros->conexion;

    if (conexion_kernel == -1) {log_info(logger, "Error con la conexión al kernel"); return NULL;}
    else {log_info(logger, "Se conectó el kernel!");}

    while(conexion_kernel >= 0)
    {
        char* mensaje = recibir_mensaje(conexion_kernel);
        log_info(logger, "%s", mensaje);
        //t_paquete* paquete = recibir_paquete(conexion_kernel);
        //log_info(logger, "Recibí de kernel: %i", paquete->codigo_operacion);
    }
    return  NULL;   
}

void* comunicacion_con_cpu(void* arg)
{

    parametros_de_hilo* parametros = (parametros_de_hilo*)arg;

    int conexion_cpu = parametros->conexion;

    if(conexion_cpu == -1)
    {
        log_info(logger, "Error conectandose con la CPU");
        return NULL;
    }
    else{log_info(logger, "Se conectó el CPU!");}


    while(conexion_cpu >= 0)
    {
        t_paquete* peticion = recibir_paquete(conexion_cpu);
        t_paquete* paquete_respuesta = crear_paquete();

        log_info(logger, "MEMORIA recibió una petición del CPU");

        switch(peticion->codigo_operacion)
        {
            case PETICION_LECTURA: //Caso lectura
                paquete_respuesta->codigo_operacion = 0; 
                enviar_paquete(paquete_respuesta, conexion_cpu);
                log_info(logger, "MEMORIA respondió una petición de lectura del CPU");
                break;
            case PETICION_ESCRITURA: //Caso escritura
                paquete_respuesta->codigo_operacion = 1;
                enviar_paquete(paquete_respuesta, conexion_cpu);
                log_info(logger, "MEMORIA respondió una petición de escritura del CPU");
                break;
            default:
                break;
        }
    }
    return NULL;
}

