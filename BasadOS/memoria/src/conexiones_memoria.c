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
    t_paquete *paquete;

    while (conexion_kernel)
    {
        paquete = recibir_paquete(conexion_kernel);
        if (paquete == NULL)
        {
            return NULL;
        }
        t_contexto_de_ejecucion *contexto = deserializar_contexto_de_ejecucion(paquete->buffer);
        t_paquete *paquete_respuesta = crear_paquete();

        switch (paquete->codigo_operacion)
        {
        case INICIALIZAR_PROCESO:
            log_info(logger, "Creación de Proceso PID: %i", contexto->pid);
            list_add(contexto->tabla_segmentos, segmento_0);
            paquete_respuesta->codigo_operacion = 0;
            paquete_respuesta->buffer = serializar_contexto(contexto);
            enviar_paquete(paquete_respuesta, conexion_kernel);
            break;
        case FINALIZAR_PROCESO:
            log_info(logger, "Eliminación de Proceso PID: %i", contexto->pid);
            int tabla_size = list_size(contexto->tabla_segmentos);
            for (int i = tabla_size - 1; i > 0; i--)
            {
                Segmento *segmento = list_get(contexto->tabla_segmentos, i);
                eliminar_segmento(contexto, segmento->id);
            }
            break;
        case CREAR_SEGMENTO:
            char *id = recibir_mensaje(conexion_kernel);
            char *tamanio = recibir_mensaje(conexion_kernel);

            int id_int = atoi(id);
            int tamanio_int = atoi(tamanio);
            free(id);
            free(tamanio);

            Segmento *segmento_nuevo = crear_segmento(id_int, tamanio_int, contexto->pid);
            if (segmento_nuevo->tamano > 0)
            {
                log_info(logger, "PID: %i - Crear Segmento: %i - Base: %i - TAMAÑO: %i", contexto->pid, segmento_nuevo->id, segmento_nuevo->desplazamiento, segmento_nuevo->tamano);
            }
            paquete_respuesta->codigo_operacion = respuesta_a_kernel(segmento_nuevo, contexto);
            paquete_respuesta->buffer = serializar_contexto(contexto);

            enviar_paquete(paquete_respuesta, conexion_kernel);
            if (segmento_nuevo->tamano == -1)
            {
                t_paquete *paquete_compactacion = crear_paquete();
                free(recibir_mensaje(conexion_kernel));
                log_info(logger, "Solicitud de Compactación");
                t_list *segmentos_modificados = compactar();
                sleep(retardo_compactacion);
                for (int i = 0; i < list_size(lista_de_memoria); i++)
                {
                    Segmento *segmento_a_loggear = list_get(lista_de_memoria, i);
                    log_info(logger, "PID: %i - Segmento: %i - Base: %i- Tamaño : %i", segmento_a_loggear->pid, segmento_a_loggear->id, segmento_a_loggear->desplazamiento, segmento_a_loggear->tamano);
                }

                paquete_compactacion->codigo_operacion = 0;
                paquete_compactacion->buffer = serializar_lista_segmentos(segmentos_modificados);

                enviar_paquete(paquete_compactacion, conexion_kernel);
                eliminar_paquete(paquete_compactacion);
                list_destroy(segmentos_modificados);
            }
            if (segmento_nuevo->tamano < 0)
            {
                free(segmento_nuevo);
            }
            break;
        case ELIMINAR_SEGMENTO:
            char *id_a_eliminar = recibir_mensaje(conexion_kernel);
            int id_a_eliminar_int = atoi(id_a_eliminar);
            free(id_a_eliminar);

            eliminar_segmento(contexto, id_a_eliminar_int);

            paquete_respuesta->codigo_operacion = 0;
            paquete_respuesta->buffer = serializar_contexto(contexto);
            enviar_paquete(paquete_respuesta, conexion_kernel);
            break;

        default:
            break;
        }

        eliminar_paquete(paquete);
        eliminar_paquete(paquete_respuesta);
        liberar_contexto_de_ejecucion(contexto);
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
    char *registro;
    char *char_dir_fis;
    int direccion_fisica;

    while (conexion_cpu)
    {
        t_paquete *peticion = recibir_paquete(conexion_cpu);
        if (peticion == NULL)
        {
            break;
        }
        t_paquete *paquete_respuesta = crear_paquete();

        t_contexto_de_ejecucion *contexto = deserializar_contexto_de_ejecucion(peticion->buffer);

        switch (peticion->codigo_operacion)
        {
        case PETICION_LECTURA: // Caso lectura, mov_in, guardo en el registro lo que lei en la direccion de memoria
            paquete_respuesta->codigo_operacion = 0;
            paquete_respuesta->buffer = serializar_contexto(contexto);
            registro = recibir_mensaje(conexion_cpu);
            char_dir_fis = recibir_mensaje(conexion_cpu);

            direccion_fisica = atoi(char_dir_fis);
            log_info(logger, "PID: %i - Acción: LEER - Dirección física: %s - Tamaño: %i- Origen: CPU", contexto->pid, char_dir_fis, tamanio_del_registro(registro));

            // Accedo a la memoria, copio los datos en una variable auxiliar
            sleep(retardo_acceso_memoria);

            char *datos_leidos = malloc(tamanio_del_registro(registro));

            memcpy(datos_leidos, espacio_de_memoria + direccion_fisica, tamanio_del_registro(registro));
            guardar_en_registros(registro, datos_leidos, contexto->registros);
            log_info(logger, "Memoria leyo %s", datos_leidos);
            free(datos_leidos);
            free(registro);
            free(char_dir_fis);
            enviar_paquete(paquete_respuesta, conexion_cpu);

            break;

        case PETICION_ESCRITURA: // Caso escritura mov_out

            char_dir_fis = recibir_mensaje(conexion_cpu);
            direccion_fisica = atoi(char_dir_fis);
            registro = recibir_mensaje(conexion_cpu);

            int tamanio_registro = tamanio_del_registro(registro);
            log_info(logger, "PID: %i - Acción: ESCRIBIR - Dirección física: %s - Tamaño: %i- Origen: CPU", contexto->pid, char_dir_fis, tamanio_del_registro(registro));

            char *datos_en_registro = leer_registro(registro, contexto->registros);

            memcpy(espacio_de_memoria + direccion_fisica, datos_en_registro, tamanio_registro);
            free(registro);
            free(char_dir_fis);
            free(datos_en_registro);

            // Paso 3: informo a CPU que la escritura ocurrió exitosamente
            paquete_respuesta->codigo_operacion = 0;
            paquete_respuesta->buffer = serializar_contexto(contexto);
            enviar_paquete(paquete_respuesta, conexion_cpu);
            break;
        default:
            break;
        }
        liberar_contexto_de_ejecucion(contexto);
        eliminar_paquete(peticion);
        eliminar_paquete(paquete_respuesta);
    }

    return NULL;
}

void *comunicacion_con_filesystem(void *arg)
{
    parametros_de_hilo *parametros = (parametros_de_hilo *)arg;

    int conexion_filesystem = parametros->conexion;

    if (conexion_filesystem == -1)
    {
        log_info(logger, "Error conectandose con el filesystem");
        return NULL;
    }
    else
    {
        log_info(logger, "Se conectó el filesystem!");
    }

    while (conexion_filesystem)
    {
        t_paquete *peticion = recibir_paquete(conexion_filesystem);
        if (peticion == NULL)
        {
            break;
        }
        t_contexto_de_ejecucion *contexto = deserializar_contexto_de_ejecucion(peticion->buffer);

        char *direccion_fisica_str;
        int direccion_fisica_valor;

        switch (peticion->codigo_operacion)
        {
        case PETICION_ESCRITURA:

            direccion_fisica_str = recibir_mensaje(conexion_filesystem);
            direccion_fisica_valor = atoi(direccion_fisica_str);
            char *datos_a_guardar = recibir_mensaje(conexion_filesystem);
            memcpy(espacio_de_memoria + direccion_fisica_valor, datos_a_guardar, strlen(datos_a_guardar));
            log_info(logger, "PID: %i - Acción: ESCRIBIR - Dirección física: %s - Tamaño: %li- Origen: FS", contexto->pid, direccion_fisica_str, strlen(datos_a_guardar));
            free(direccion_fisica_str);
            enviar_mensaje("OK!", conexion_filesystem);
            break;

        case PETICION_LECTURA:
            direccion_fisica_str = recibir_mensaje(conexion_filesystem);
            direccion_fisica_valor = atoi(direccion_fisica_str);

            char *cantidad_bytes_str = recibir_mensaje(conexion_filesystem);
            int cantidad_bytes_valor = atoi(cantidad_bytes_str);
            log_info(logger, "PID: %i - Acción: LEER - Dirección física: %s - Tamaño: %i- Origen: FS", contexto->pid, direccion_fisica_str, cantidad_bytes_valor);

            char *datos = malloc((cantidad_bytes_valor + 1) * sizeof(char));
            memcpy(datos, espacio_de_memoria + direccion_fisica_valor, cantidad_bytes_valor * sizeof(char));
            datos[cantidad_bytes_valor] = '\0';

            enviar_mensaje(datos, conexion_filesystem);
            log_info(logger, "MEMORIA leyo %s",datos);
            free(datos);
            free(direccion_fisica_str);
            free(cantidad_bytes_str);

            break;
        default:
            break;
        }
        eliminar_paquete(peticion);
    }
    return NULL;
}

op_code respuesta_a_kernel(Segmento *segmento, t_contexto_de_ejecucion *contexto)
{
    int cantidad_segmentos = list_size(contexto->tabla_segmentos);
    int tamano_segmento = segmento->tamano;

    if (tamano_segmento == -2 || cantidad_segmentos == cantidad_maxima_segmentos_por_proceso)
    {
        return OUT_OF_MEMORY;
    }
    else if (tamano_segmento > 0)
    {
        list_add(contexto->tabla_segmentos, segmento);
        return SEGMENTO_CREADO;
    }
    else if (tamano_segmento == -1)
    {
        return COMPACTACION_NECESARIA;
    }

    return 0;
}
