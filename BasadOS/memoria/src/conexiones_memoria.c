#include "conexiones_memoria.h"

void *comunicacion_con_kernel(void *arg)
{
    parametros_de_hilo *parametros = (parametros_de_hilo *)arg;

    int conexion_kernel = parametros->conexion;

    if (conexion_kernel == -1)
    {
        log_info(logger, "Error con la conexión al kernel");
        return NULL;
    }
    else
    {
        log_info(logger, "Se conectó el kernel!");
    }

    while (conexion_kernel >= 0)
    {
        log_info(logger, "Recibi una peticion de KERNEL");
        t_paquete *paquete = recibir_paquete(conexion_kernel);
        t_contexto_de_ejecucion *contexto = deserializar_contexto_de_ejecucion(paquete->buffer);

        switch (paquete->codigo_operacion)
        {
        case INICIALIZAR_PROCESO:
            list_add(contexto->tabla_segmentos, segmento_0);
            t_paquete *paquete_inicializar_proceso = crear_paquete();
            paquete_inicializar_proceso->codigo_operacion = 0;
            paquete_inicializar_proceso->buffer = serializar_contexto(contexto);
            enviar_paquete(paquete_inicializar_proceso, conexion_kernel);
            free(paquete_inicializar_proceso);
            break;
        case FINALIZAR_PROCESO:
            finalizar_proceso(contexto);
            break;
        case CREAR_SEGMENTO:
            char *id = recibir_mensaje(conexion_kernel);
            log_info(logger, "El id es: %s", id);
            char *tamanio = recibir_mensaje(conexion_kernel);
            log_info(logger, "El tamanio es: %s", tamanio);

            int id_int = atoi(id);
            int tamanio_int = atoi(tamanio);

            Segmento *segmento_nuevo = crear_segmento(id_int, tamanio_int);

            log_info(logger, "EL TAMANIO DEL SEGMENTO CREADO ES: %d", segmento_nuevo->tamano);

            t_paquete *paquete_a_kernel = crear_paquete();
            paquete_a_kernel->codigo_operacion = respuesta_a_kernel(segmento_nuevo, contexto);
            paquete_a_kernel->buffer = serializar_contexto(contexto);
            for (int i = 0; i < list_size(contexto->tabla_segmentos); i++)
            {
                Segmento *sas = list_get(contexto->tabla_segmentos, i);
                log_info(logger, "SEGMENTO ID: %d, DESPLAZAMIENTO: %d", sas->id, sas->desplazamiento);
            }

            enviar_paquete(paquete_a_kernel, conexion_kernel);

            if (segmento_nuevo->tamano == -1)
            {
                recibir_mensaje(conexion_kernel);
                log_info(logger, "SE pidio compactar");
                t_list *segmentos_modificados = compactar();
                paquete_a_kernel->codigo_operacion = 0;
                paquete_a_kernel->buffer = serializar_lista_segmentos(segmentos_modificados);

                enviar_paquete(paquete_a_kernel, conexion_kernel);
            }

            break;
        case ELIMINAR_SEGMENTO:
            log_info(logger, "SE pidio eliminar un segmento");

            char *id_a_eliminar = recibir_mensaje(conexion_kernel);
            log_info(logger, "El id es: %s", id_a_eliminar);
            int id_a_eliminar_int = atoi(id_a_eliminar);
            int posicion = obtener_segmento_por_id(id_a_eliminar_int, contexto->tabla_segmentos);
            log_info(logger, "La posicion es: %d", posicion);
            eliminar_segmento(id_a_eliminar_int);
            list_remove(contexto->tabla_segmentos, posicion);

            t_paquete *paquete_a_kernel_eliminar = crear_paquete();
            paquete_a_kernel_eliminar->codigo_operacion = 0;
            paquete_a_kernel_eliminar->buffer = serializar_contexto(contexto);
            enviar_paquete(paquete_a_kernel_eliminar, conexion_kernel);
            break;

    
        default:
            break;
        }
        liberar_contexto_de_ejecucion(contexto);
        eliminar_paquete(paquete);
    }
    return NULL;
}

void *comunicacion_con_cpu(void *arg)
{

    parametros_de_hilo *parametros = (parametros_de_hilo *)arg;

    int conexion_cpu = parametros->conexion;

    if (conexion_cpu == -1)
    {
        log_info(logger, "Error conectandose con la CPU");
        return NULL;
    }
    else
    {
        log_info(logger, "Se conectó el CPU!");
    }

    while (conexion_cpu >= 0)
    {
        t_paquete *peticion = recibir_paquete(conexion_cpu);
        t_contexto_de_ejecucion *contexto = deserializar_contexto_de_ejecucion(peticion->buffer);
        t_paquete *paquete_respuesta = crear_paquete();

        char *registro;
        char* char_dir_fis;
        int direccion_fisica;

        log_info(logger, "MEMORIA recibió una petición del CPU");

        switch (peticion->codigo_operacion)
        {
        case PETICION_LECTURA: // Caso lectura, mov_in, guardo en el registro lo que lei en la direccion de memoria
            paquete_respuesta->codigo_operacion = 0;
            paquete_respuesta->buffer = serializar_contexto(contexto);
            registro = recibir_mensaje(conexion_cpu);
            char_dir_fis = recibir_mensaje(conexion_cpu);
            direccion_fisica = atoi(char_dir_fis);


            // Accedo a la memoria, copio los datos en una variable auxiliar
            sleep(retardo_acceso_memoria);
            log_info(logger, "Retardo el acceso a memoria %i segundos...", retardo_acceso_memoria);
            char *datos_leidos = malloc(tamanio_del_registro(registro));
            memcpy(datos_leidos, espacio_de_memoria + direccion_fisica, tamanio_del_registro(registro));

            guardar_en_registros(registro, datos_leidos, contexto->registros);

            log_info(logger, "Guarde en el registro %s", leer_registro(registro, contexto->registros)); //CORREGIR

            enviar_paquete(paquete_respuesta, conexion_cpu);
            log_info(logger, "MEMORIA respondió una petición de lectura del CPU");

            break;

        case PETICION_ESCRITURA: // Caso escritura mov_out
            log_info(logger, "MEMORIA recibió una petición de escritura del CPU");

            char_dir_fis = recibir_mensaje(conexion_cpu);
            direccion_fisica = atoi(char_dir_fis);
            registro = recibir_mensaje(conexion_cpu);

            int tamanio_registro = tamanio_del_registro(registro);

            log_info(logger, "Retardo el acceso a memoria %i segundos...", retardo_acceso_memoria);
            char* datos_en_registro = malloc(tamanio_registro * sizeof(char));
            datos_en_registro = leer_registro(registro, contexto->registros);
            log_info(logger, "MOV_OUT va a guardar %s", datos_en_registro);

            memcpy(espacio_de_memoria + direccion_fisica, datos_en_registro, tamanio_registro);

            char *test = malloc(tamanio_registro);
            memcpy(test, espacio_de_memoria + direccion_fisica, tamanio_registro);
            log_info(logger, "Guarde en memoria %s", test);

            // Paso 3: informo a CPU que la escritura ocurrió exitosamente
            paquete_respuesta->codigo_operacion = 0;
            paquete_respuesta->buffer = serializar_contexto(contexto);
            enviar_paquete(paquete_respuesta, conexion_cpu);
            break;
        default:
            break;
        }
    }
    return NULL;
}

// TODO: esto hay que cambiarlo a un enum
op_code respuesta_a_kernel(Segmento *segmento, t_contexto_de_ejecucion *contexto)
{
    if (segmento->tamano == -2 || list_size(contexto->tabla_segmentos) > cantidad_maxima_segmentos_por_proceso)
    {
        return OUT_OF_MEMORY;
    }
    else if (segmento->tamano > 0)
    {
        list_add(contexto->tabla_segmentos, segmento);
        return SEGMENTO_CREADO;
    }
    else if (segmento->tamano == -1)
    {
        return COMPACTACION_NECESARIA;
    }

    return 0;
}

void finalizar_proceso(t_contexto_de_ejecucion *contexto_de_ejecucion)
{
    int tabla_size = list_size(contexto_de_ejecucion->tabla_segmentos);
    for (int i = tabla_size - 1; i > 0; i--)
    {
        Segmento *segmento = list_remove(contexto_de_ejecucion->tabla_segmentos, i);
        eliminar_segmento(segmento->id);
    }
}