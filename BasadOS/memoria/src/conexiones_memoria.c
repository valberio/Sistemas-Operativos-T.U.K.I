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

    while (conexion_kernel)
    {
        log_info(logger, "Recibi una peticion de KERNEL");
        t_paquete *paquete = recibir_paquete(conexion_kernel);
        if(paquete == NULL){
            break;
        }
        t_contexto_de_ejecucion *contexto = deserializar_contexto_de_ejecucion(paquete->buffer);
   
        switch (paquete->codigo_operacion)
        {
        case INICIALIZAR_PROCESO:
            log_info(logger, "Creación de Proceso PID: %i", contexto->pid);
            list_add(contexto->tabla_segmentos, segmento_0);
            t_paquete *paquete_inicializar_proceso = crear_paquete();
            paquete_inicializar_proceso->codigo_operacion = 0;
            paquete_inicializar_proceso->buffer = serializar_contexto(contexto);
            enviar_paquete(paquete_inicializar_proceso, conexion_kernel);
            eliminar_paquete(paquete_inicializar_proceso);
            break;
        case FINALIZAR_PROCESO:
            log_info(logger, "Eliminación de Proceso PID: %i", contexto->pid);
            finalizar_proceso(contexto);
            break;
        case CREAR_SEGMENTO:
            char *id = recibir_mensaje(conexion_kernel);
            char *tamanio = recibir_mensaje(conexion_kernel);

            int id_int = atoi(id);
            int tamanio_int = atoi(tamanio);

            Segmento *segmento_nuevo = crear_segmento(id_int, tamanio_int, contexto->pid);
            if (segmento_nuevo->tamano > 0)
            {
                log_info(logger, "PID: %i - Crear Segmento: %i - Base: %i - TAMAÑO: %i", contexto->pid, segmento_nuevo->id, segmento_nuevo->desplazamiento, segmento_nuevo->tamano);
            }
            t_paquete *paquete_a_kernel = crear_paquete();
            paquete_a_kernel->codigo_operacion = respuesta_a_kernel(segmento_nuevo, contexto);
            paquete_a_kernel->buffer = serializar_contexto(contexto);

            enviar_paquete(paquete_a_kernel, conexion_kernel);
            eliminar_paquete(paquete_a_kernel);
            if (segmento_nuevo->tamano == -1)
            {
                recibir_mensaje(conexion_kernel);
                log_info(logger, "Solicitud de Compactación");
                t_list *segmentos_modificados = compactar();
                for (int i = 0; i < list_size(lista_de_memoria); i++)
                {
                    Segmento *segmento_a_loggear = list_get(lista_de_memoria, i);
                    log_info(logger, "PID: %i - Segmento: %i - Base: %i- Tamaño : %i", segmento_a_loggear->pid,segmento_a_loggear->id, segmento_a_loggear->desplazamiento,segmento_a_loggear->tamano);
                }

                t_paquete *paquete_compactacion = crear_paquete();
                paquete_compactacion->codigo_operacion = 0;
                paquete_compactacion->buffer = serializar_lista_segmentos(segmentos_modificados);

                enviar_paquete(paquete_compactacion, conexion_kernel);
                eliminar_paquete(paquete_compactacion);
            }

            break;
        case ELIMINAR_SEGMENTO:
            char *id_a_eliminar = recibir_mensaje(conexion_kernel);
            int id_a_eliminar_int = atoi(id_a_eliminar);
            int posicion = obtener_segmento_por_id(id_a_eliminar_int, contexto->tabla_segmentos);

            eliminar_segmento(contexto, id_a_eliminar_int);
            list_remove(contexto->tabla_segmentos, posicion);

            t_paquete *paquete_a_kernel_eliminar = crear_paquete();
            paquete_a_kernel_eliminar->codigo_operacion = 0;
            paquete_a_kernel_eliminar->buffer = serializar_contexto(contexto);
            enviar_paquete(paquete_a_kernel_eliminar, conexion_kernel);
            eliminar_paquete(paquete_a_kernel_eliminar);
            break;

        default:
            break;
        }

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

    while (conexion_cpu)
    {
        t_paquete *peticion = recibir_paquete(conexion_cpu);
        if(peticion == NULL){
            break;
        }
        t_contexto_de_ejecucion *contexto = deserializar_contexto_de_ejecucion(peticion->buffer);
        t_paquete *paquete_respuesta = crear_paquete();

        char *registro;
        char *char_dir_fis;
        int direccion_fisica;
        
  
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
            log_info(logger, "Retardo el acceso a memoria %i segundos...", retardo_acceso_memoria);

            char *datos_leidos = malloc(tamanio_del_registro(registro));
            
            memcpy(datos_leidos, espacio_de_memoria + direccion_fisica, tamanio_del_registro(registro));
            log_info(logger, "Lei de memoria %s", datos_leidos);
            guardar_en_registros(registro, datos_leidos, contexto->registros);

            log_info(logger, "Guarde en el registro %s", leer_registro(registro, contexto->registros));

            enviar_paquete(paquete_respuesta, conexion_cpu);
            log_info(logger, "MEMORIA respondió una petición de lectura del CPU");

            break;

        case PETICION_ESCRITURA: // Caso escritura mov_out

            char_dir_fis = recibir_mensaje(conexion_cpu);
            direccion_fisica = atoi(char_dir_fis);
            registro = recibir_mensaje(conexion_cpu);

            int tamanio_registro = tamanio_del_registro(registro);
            log_info(logger, "PID: %i - Acción: ESCRIBIR - Dirección física: %s - Tamaño: %i- Origen: CPU", contexto->pid, char_dir_fis, tamanio_del_registro(registro));

            char *datos_en_registro = leer_registro(registro, contexto->registros);
            

            memcpy(espacio_de_memoria + direccion_fisica, datos_en_registro, tamanio_registro);
 
            char *test = malloc((tamanio_registro)* sizeof(char));
            memcpy(test, espacio_de_memoria + direccion_fisica, tamanio_registro);
            log_info(logger, "MOV OUT escribio en memoria %s", test);
            free(test);
            // Paso 3: informo a CPU que la escritura ocurrió exitosamente
            paquete_respuesta->codigo_operacion = 0;
            paquete_respuesta->buffer = serializar_contexto(contexto);
            enviar_paquete(paquete_respuesta, conexion_cpu);
            break;
        default:
            break;
        }
        
        eliminar_paquete(paquete_respuesta);
        eliminar_paquete(peticion);

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
        if(peticion == NULL){
            break;
        }
        t_contexto_de_ejecucion *contexto = deserializar_contexto_de_ejecucion(peticion->buffer);
        // t_paquete *paquete_respuesta = crear_paquete();

        char *dir_fis;
        int direccion_fisica;

        switch (peticion->codigo_operacion)
        {
        case PETICION_ESCRITURA:

            dir_fis = recibir_mensaje(conexion_filesystem);
            direccion_fisica = atoi(dir_fis);
            char *datos_a_guardar = recibir_mensaje(conexion_filesystem);
            memcpy(espacio_de_memoria + direccion_fisica, datos_a_guardar, strlen(datos_a_guardar));
            log_info(logger, "PID: %i - Acción: ESCRIBIR - Dirección física: %s - Tamaño: %li- Origen: FS", contexto->pid, dir_fis, strlen(datos_a_guardar));

            char *test = malloc(strlen(datos_a_guardar));
            memcpy(test, espacio_de_memoria + direccion_fisica, strlen(datos_a_guardar));

            log_info(logger, "Memoria guardo %s", test);

            enviar_mensaje("OK!", conexion_filesystem);
            break;

        case PETICION_LECTURA:
            dir_fis = recibir_mensaje(conexion_filesystem);
            direccion_fisica = atoi(dir_fis);
            char *cant_b = recibir_mensaje(conexion_filesystem);
            int cantidad_bytes = atoi(cant_b);
            log_info(logger, "PID: %i - Acción: LECTURA - Dirección física: %s - Tamaño: %i- Origen: FS", contexto->pid, dir_fis, cantidad_bytes);

            char *datos = malloc((cantidad_bytes + 1) * sizeof(char));
            memcpy(datos, espacio_de_memoria + direccion_fisica, cantidad_bytes * sizeof(char));
            datos[cantidad_bytes] = '\0'; 
            log_info(logger, "Lei %s porque me lo pidio filesystem", datos);

            enviar_mensaje(datos, conexion_filesystem);
            free(datos);
            break;
        default:
            break;
        }
        eliminar_paquete(peticion);
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
        Segmento *segmento = list_get(contexto_de_ejecucion->tabla_segmentos, i);
        log_info(logger, "El segmento es: %i", segmento->id);
        eliminar_segmento(contexto_de_ejecucion, segmento->id);
    }
}