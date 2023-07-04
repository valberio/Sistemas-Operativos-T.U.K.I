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

            enviar_paquete(paquete_a_kernel, conexion_kernel);

            if (segmento_nuevo->tamano == -1)
            {
                recibir_mensaje(conexion_kernel);
                log_info(logger, "SE pidio compactar");
                compactar();
                log_info(logger, "LA LISTA DE MEMORIA TIENE: ");
                for (int i = 0; i < list_size(lista_de_memoria); i++)
                {
                    Segmento *sas = list_get(lista_de_memoria, i);
                    log_info(logger, "SEGMENTO ID: %d, DESPLAZAMIENTO: %d", sas->id, sas->desplazamiento);
                }
                log_info(logger, "LA TABLA DE SEGMENTOS DEL CONTEXTO  TIENE: ");
                for (int i = 0; i < list_size(contexto->tabla_segmentos); i++)
                {
                    Segmento *sas = list_get(contexto->tabla_segmentos, i);
                    log_info(logger, "SEGMENTO ID: %d, DESPLAZAMIENTO: %d", sas->id, sas->desplazamiento);
                }
                enviar_mensaje("Listo", conexion_kernel);
            }

            break;
        case ELIMINAR_SEGMENTO:
            log_info(logger, "SE pidio eliminar un segmento");

            char *id_a_eliminar = recibir_mensaje(conexion_kernel);

            int id_a_eliminar_int = atoi(id_a_eliminar);
            int posicion = get_index_of_list(contexto->tabla_segmentos, id_a_eliminar_int);
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
        char *direccion_logica;
        int direccion_logica_int;
        int direccion_fisica;

        log_info(logger, "MEMORIA recibió una petición del CPU");

        switch (peticion->codigo_operacion)
        {
        case PETICION_LECTURA: // Caso lectura, mov_in, guardo en el registro lo que lei en la direccion de memoria
            paquete_respuesta->codigo_operacion = 0;
            paquete_respuesta->buffer = serializar_contexto(contexto);
            registro = recibir_mensaje(conexion_cpu);
            direccion_logica = recibir_mensaje(conexion_cpu);
            direccion_logica_int = atoi(direccion_logica);

            direccion_fisica = traduccion_dir_logica_fisica(direccion_logica_int, contexto->tabla_segmentos);

            // Accedo a la memoria, copio los datos en una variable auxiliar
            char *datos_leidos = malloc(tamanio_del_registro(registro));
            memcpy(datos_leidos, espacio_de_memoria + direccion_fisica, tamanio_del_registro(registro));

            guardar_en_registros(registro, datos_leidos, contexto);

            log_info(logger, "Guarde en el registro %s", contexto->registros->AX);

            enviar_paquete(paquete_respuesta, conexion_cpu);
            log_info(logger, "MEMORIA respondió una petición de lectura del CPU");

            break;

        case PETICION_ESCRITURA: // Caso escritura mov_out
            log_info(logger, "MEMORIA recibió una petición de escritura del CPU");

            direccion_logica = recibir_mensaje(conexion_cpu);
            direccion_logica_int = atoi(direccion_logica);
            registro = recibir_mensaje(conexion_cpu);

            int tamanio_registro = tamanio_del_registro(registro);

            char *datos_en_registro = malloc(tamanio_registro * sizeof(char));
            datos_en_registro = leer_registro(registro, contexto);
            log_info(logger, "Voy a guardar %s", datos_en_registro);

            direccion_fisica = traduccion_dir_logica_fisica(direccion_logica_int, contexto->tabla_segmentos);

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
    if (segmento->tamano > 0)
    {
        list_add(contexto->tabla_segmentos, segmento);
        return SEGMENTO_CREADO;
    }
    else if (segmento->tamano == -1)
    {
        return COMPACTACION_NECESARIA;
    }
    else if (segmento->tamano == -2)
    {
        return OUT_OF_MEMORY;
    }
    return 0;
}
int tamanio_del_registro(char *registro_char)
{
    int tamanio_del_registro = 0;
    enum Registros registro = string_a_registro(registro_char);
    switch (registro)
    {
    case rAX:
        tamanio_del_registro = 5; // 4 + 1, para considerar el \0
        break;
    case rBX:
        tamanio_del_registro = 5;
        break;
    case rCX:
        tamanio_del_registro = 5;
        break;
    case rDX:
        tamanio_del_registro = 5;
        break;

    // Registros 8 bits
    case rEAX:
        tamanio_del_registro = 9;
        break;

    case rEBX:
        tamanio_del_registro = 9;
        break;

    case rECX:
        tamanio_del_registro = 9;
        break;

    case rEDX:
        tamanio_del_registro = 9;
        break;

    // Registros de 16 bits
    case rRAX:
        tamanio_del_registro = 17;
        break;

    case rRBX:
        tamanio_del_registro = 17;
        break;

    case rRCX:
        tamanio_del_registro = 17;
        break;

    case rRDX:
        tamanio_del_registro = 17;
        break;
    }
    return tamanio_del_registro;
}

void guardar_en_registros(char *registro_char, char *datos, t_contexto_de_ejecucion *contexto)
{
    enum Registros registro = string_a_registro(registro_char);

    switch (registro)
    {
    case rAX:
        strcpy(contexto->registros->AX, datos); // 4 + 1, para considerar el \0
        break;
    case rBX:
        strcpy(contexto->registros->BX, datos);
        break;
    case rCX:
        strcpy(contexto->registros->CX, datos);
        break;
    case rDX:
        strcpy(contexto->registros->DX, datos);
        break;

    // Registros 8 bits
    case rEAX:
        strcpy(contexto->registros->EAX, datos);
        break;

    case rEBX:
        strcpy(contexto->registros->EBX, datos);
        break;

    case rECX:
        strcpy(contexto->registros->ECX, datos);
        break;

    case rEDX:
        strcpy(contexto->registros->EDX, datos);
        break;

    // Registros de 16 bits
    case rRAX:
        strcpy(contexto->registros->RAX, datos);
        break;

    case rRBX:
        strcpy(contexto->registros->RBX, datos);
        break;

    case rRCX:
        strcpy(contexto->registros->RCX, datos);
        break;

    case rRDX:
        strcpy(contexto->registros->RDX, datos);
        break;
    }

}