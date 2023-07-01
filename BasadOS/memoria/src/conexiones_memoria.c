#include "conexiones_memoria.h"

void* comunicacion_con_kernel(void* arg)
{
    parametros_de_hilo* parametros = (parametros_de_hilo*)arg;

    int conexion_kernel = parametros->conexion;

    if (conexion_kernel == -1) {log_info(logger, "Error con la conexión al kernel"); return NULL;}
    else {log_info(logger, "Se conectó el kernel!");}

    while(conexion_kernel >= 0)
    {
        log_info(logger, "Recibi una peticion de KERNEL"); 
        t_paquete* paquete = recibir_paquete(conexion_kernel);
        t_contexto_de_ejecucion* contexto = deserializar_contexto_de_ejecucion(paquete->buffer);


        switch(paquete->codigo_operacion)
        {
            case CREAR_SEGMENTO:
                char* id = recibir_mensaje(conexion_kernel);
                char* tamanio = recibir_mensaje(conexion_kernel);

                int id_int = atoi(id);
				int tamanio_int = atoi(tamanio);

                Segmento* segmento_nuevo = crear_segmento(id_int, tamanio_int);
                char* respuesta = respuesta_a_kernel(segmento_nuevo, contexto);
                
                //TODO: esto tiene que devolver un paquete con el contexto actualizado, y en el codop, la respuesta de memoria

                enviar_mensaje(respuesta, conexion_kernel);
                log_info(logger, "MEMORIA envio el siguiente mensaje a kernel: %s",respuesta);

                t_paquete* paquete_respuesta = crear_paquete();
                paquete_respuesta->buffer = serializar_contexto(contexto);
                enviar_paquete(paquete_respuesta, conexion_kernel);

                break;
            case ELIMINAR_SEGMENTO:
                break;
            default:
                break;
        }
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
            case PETICION_LECTURA: //Caso lectura, mov_in
                paquete_respuesta->codigo_operacion = 0;


                char* registro = recibir_mensaje(conexion_cpu);
                void* direccion_fisica = recibir_mensaje(conexion_cpu);

                log_info(logger, "MEMORIA recibió el registro %s", registro);
                log_info(logger, "MEMORIA recibió la dirección %s", (char*)direccion_fisica);

                enviar_paquete(paquete_respuesta, conexion_cpu);
                log_info(logger, "MEMORIA respondió una petición de lectura del CPU");
                break;
            case PETICION_ESCRITURA: //Caso escritura mov_out
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


//TODO: esto hay que cambiarlo a un enum
char* respuesta_a_kernel(Segmento* segmento, t_contexto_de_ejecucion* contexto){
    if(segmento->tamano > 0){
        list_add(contexto->tabla_segmentos, segmento);
        return "SEGMENTO CREADO";
    }
    else if(segmento->tamano == -1){
        return "COMPACTAR";
    }
    else if(segmento->tamano == -2){
        return "OUT OF MEMORY";
    }
    return "ERROR";
}