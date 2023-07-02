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
                log_info(logger, "El id es: %s", id); 
                char* tamanio = recibir_mensaje(conexion_kernel);
                log_info(logger, "El tamanio es: %s", tamanio); 


                int id_int = atoi(id);
				int tamanio_int = atoi(tamanio);

                Segmento* segmento_nuevo = crear_segmento(id_int, tamanio_int);

                log_info(logger, "EL TAMANIO DEL SEGMENTO CREADO ES: %d", segmento_nuevo->tamano);

               	

                t_paquete* paquete_a_kernel = crear_paquete();
                log_info(logger, "En el contexto hay %i segmentos", list_size(contexto->tabla_segmentos));
				paquete_a_kernel->codigo_operacion = respuesta_a_kernel(segmento_nuevo, contexto);
				paquete_a_kernel->buffer = serializar_contexto(contexto);
                log_info(logger, "En el contexto hay %i segmentos", list_size(contexto->tabla_segmentos));

                log_info(logger,"Voy a enviar el pauqete a kernel");
                enviar_paquete(paquete_a_kernel, conexion_kernel);
                
                if(segmento_nuevo->tamano == -1){
                    recibir_mensaje(conexion_kernel);
                    log_info(logger,"SE pidio compactar");
                    compactar(); 
                    log_info(logger,"LA LISTA DE MEMORIA TIENE: ");
					for(int i = 0; i < list_size(lista_de_memoria);i++){
					    Segmento* sas =list_get(lista_de_memoria,i);
						log_info(logger,"SEGMENTO ID: %d, DESPLAZAMIENTO: %d",sas->id,sas->desplazamiento);
					}		
                    enviar_mensaje("Listo",conexion_kernel);
                }

                break;
            case ELIMINAR_SEGMENTO:
                log_info(logger, "SE pidio eliminar un segmento"); 

                char* id_a_eliminar = recibir_mensaje(conexion_kernel);
                log_info(logger, "El id es: %s", id_a_eliminar); 


                int id_a_eliminar_int = atoi(id_a_eliminar);
                Segmento *test = list_get(contexto->tabla_segmentos, 0);
                int posicion = get_index_of_list(contexto->tabla_segmentos, id_a_eliminar_int);
                log_info(logger, "La posicion es: %d", posicion); 
                eliminar_segmento(id_a_eliminar_int);
                list_remove(contexto->tabla_segmentos,posicion);

                
                t_paquete* paquete_a_kernel_eliminar = crear_paquete();
                paquete_a_kernel_eliminar->codigo_operacion = 0;
                paquete_a_kernel_eliminar->buffer = serializar_contexto(contexto);
                enviar_paquete(paquete_a_kernel_eliminar, conexion_kernel);
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
        t_contexto_de_ejecucion* contexto = deserializar_contexto_de_ejecucion(peticion->buffer);
        t_paquete* paquete_respuesta = crear_paquete();

        char* registro;
        char* direccion_fisica;

        log_info(logger, "MEMORIA recibió una petición del CPU");

        switch(peticion->codigo_operacion)
        {
            case PETICION_LECTURA: //Caso lectura, mov_in
                paquete_respuesta->codigo_operacion = 0;

                registro = recibir_mensaje(conexion_cpu);
                direccion_fisica = recibir_mensaje(conexion_cpu);

                log_info(logger, "MEMORIA recibió el registro %s", registro);
                log_info(logger, "MEMORIA recibió la dirección %s", (char*)direccion_fisica);

                enviar_paquete(paquete_respuesta, conexion_cpu);
                log_info(logger, "MEMORIA respondió una petición de lectura del CPU");
    
                break;
            case PETICION_ESCRITURA: //Caso escritura mov_out
                log_info(logger, "MEMORIA respondió una petición de escritura del CPU");

                direccion_fisica = recibir_mensaje(conexion_cpu);
                int dir = atoi(direccion_fisica);
                registro = recibir_mensaje(conexion_cpu);

                log_info(logger, "CPU me pidio MOV_OUT, hay %i segmentos en la tabla de segmentos", list_size(contexto->tabla_segmentos));

                char* datos_en_registro = leer_registro(registro, contexto);
                
                int dir_fis = traduccion_dir_logica_fisica(direccion_fisica, contexto);
                
                //Paso 3: informo a CPU que la escritura ocurrió exitosamente
                log_info(logger, "Lei del registro %s el valor %s", registro, datos_en_registro);
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


//TODO: esto hay que cambiarlo a un enum
op_code respuesta_a_kernel(Segmento* segmento, t_contexto_de_ejecucion* contexto){
    if(segmento->tamano > 0){
        list_add(contexto->tabla_segmentos, segmento);
        return SEGMENTO_CREADO;
    }
    else if(segmento->tamano == -1){
        return COMPACTACION_NECESARIA;
    }
    else if(segmento->tamano == -2){
        return OUT_OF_MEMORY;
    }
    
}