#include "pcb.h"

int contador = 0;

t_pcb * crear_pcb( char* instrucciones, int socket, double estimado_rafaga)
{
    t_pcb *pcb = malloc(sizeof(t_pcb));
    pcb->socket_consola = socket;
	pcb->estado = 1;
	pcb->pid = contador;
    pcb->contexto_de_ejecucion = crear_contexto_de_ejecucion(instrucciones);
    pcb->contexto_de_ejecucion->pid = contador;
	pcb->tabla_archivos_abiertos = list_create();
	pcb->tabla_segmentos = list_create();


	pcb->estimado_rafaga = estimado_rafaga;
	
    contador++;
    
	return pcb;
}

t_contexto_de_ejecucion* crear_contexto_de_ejecucion(char* instrucciones){
    t_contexto_de_ejecucion* contexto = malloc(sizeof(t_contexto_de_ejecucion));

    contexto->program_counter = 0;
    contexto->registros = malloc(sizeof(t_registros));

    instanciar_registros(contexto->registros);

    contexto->instrucciones = list_create();
    t_list* lista_instrucciones = string_a_lista(instrucciones);
    list_add_all(contexto->instrucciones, lista_instrucciones);
    
    contexto->cant_instrucciones = list_size(contexto->instrucciones); //Funciona

    contexto->largo_instruccion = malloc(sizeof(uint32_t) * contexto->cant_instrucciones);

    //Lleno el array de largos de instruccion con los datos
    for (int i = 0; i < contexto->cant_instrucciones; i++)
    {   
        contexto->largo_instruccion[i] = strlen(list_get(contexto->instrucciones, i)) + 1;
    }
    contexto->tabla_segmentos = list_create();
    return contexto;

}

 t_list* string_a_lista(char* str) //Testeada y funcionando
{
    if (str == NULL) {
        // Manejo de cadena nula
        return NULL;
    }

    t_list* temp_list = list_create();
    char* temp_str = strdup(str); // Realizar una copia de la cadena

    char* token = strtok(temp_str, "\n");

    while (token != NULL)
    {
        if (strlen(token) > 0) {
            // Agregar token no vacío a la lista
            char* token_copy = strdup(token); // Realizar una copia del token

            // Agregar el carácter nulo al final del token copiado
            size_t token_length = strlen(token_copy);
            token_copy[token_length] = '\0';

            list_add(temp_list, token_copy);
        }

        token = strtok(NULL, "\n");
    }

    free(temp_str); // Liberar memoria asignada a la copia de la cadena

    return temp_list;
}

void liberar_pcb(t_pcb* pcb)
{
	list_destroy_and_destroy_elements(pcb->contexto_de_ejecucion->instrucciones, free);
	list_destroy_and_destroy_elements(pcb->tabla_segmentos, free);
	list_destroy_and_destroy_elements(pcb->tabla_archivos_abiertos, free);
	free(pcb->contexto_de_ejecucion->registros);
	free(pcb);
}

void liberar_contexto_de_ejecucion(t_contexto_de_ejecucion* contexto_de_ejecucion){
	list_clean(contexto_de_ejecucion->instrucciones);
	list_destroy(contexto_de_ejecucion->instrucciones);
	free(contexto_de_ejecucion->registros);
	free(contexto_de_ejecucion);
}


void enviar_contexto_de_ejecucion(t_contexto_de_ejecucion* contexto,int conexion_socket)
{
    t_paquete* paquete = crear_paquete();
    t_buffer* buffer = serializar_contexto(contexto);

    paquete->buffer = buffer;
    paquete->codigo_operacion = 0;

    enviar_paquete(paquete, conexion_socket);
    eliminar_paquete(paquete);
}

t_paquete* recibir_contexto_de_ejecucion(int conexion_socket){
    t_paquete* paquete = crear_paquete();
    //t_contexto_de_ejecucion* contexto = malloc(sizeof(t_contexto_de_ejecucion));
    
    recv(conexion_socket, &(paquete->codigo_operacion), sizeof(int), MSG_WAITALL);
    recv(conexion_socket, &(paquete->buffer->size), sizeof(uint32_t), MSG_WAITALL);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    recv(conexion_socket, paquete->buffer->stream, paquete->buffer->size, MSG_WAITALL);

    return paquete;
    //contexto = deserializar_contexto_de_ejecucion(paquete->buffer);
    

    /*if(contexto == NULL)
    {
        printf("El buffer es nulo\n");
    }*/

    //return contexto;
}

void instanciar_registros(t_registros* registro)
{
    strcpy(registro->AX, "0000");
	strcpy(registro->BX, "0000");
	strcpy(registro->CX, "0000");
	strcpy(registro->DX, "UNGA");
	
	strcpy(registro->EAX, "00000000");
	strcpy(registro->EBX, "00000000");
	strcpy(registro->ECX, "00000000");
	strcpy(registro->EDX, "00000000");

    strcpy(registro->RAX, "0000000000000000");
	strcpy(registro->RBX, "0000000000000000");
	strcpy(registro->RCX, "0000000000000000");
	strcpy(registro->RDX, "0000000000000000");
}

t_buffer* serializar_contexto(t_contexto_de_ejecucion* contexto)
{
    t_buffer* buffer = malloc(sizeof(t_buffer));

    //Calculo el tamaño que necesito darle al buffer.
    uint32_t tamano = 0;

    //0 Tamaño del PID
    tamano += sizeof(uint32_t);

    //0.1 Tamaño del PROGRAM COUNTER
    tamano += sizeof(uint32_t);

    //0.2 Tamaño de los registros
    tamano += sizeof(t_registros);

    //1. Tamaño de las INSTRUCCIONES de la lista
    for (int i = 0; i < contexto->cant_instrucciones; i++)
    {
        tamano += strlen(list_get(contexto->instrucciones, i)) + 1;
    }

    //2. NO tengo que guardar el puntero de la lista, creo la lista y le guardo
    //las instrucciones en el momento de deserializar.

    //3. Tamaño del CANTIDAD INSTRUCCIONES
    tamano += sizeof(uint32_t);

    //4. Tamaño del ARRAY del largo de cada instruccion
    //Sé que el array de largos va a tener la misma cant. de elemntos que cant_instrucciones
    for(int i = 0; i < contexto->cant_instrucciones; i++)
    {
        tamano += sizeof(uint32_t);
    }

    //5. Tamaño de la TABLA de segmentos
    tamano += sizeof(int) + (3 * sizeof(int)) * list_size(contexto->tabla_segmentos);  


    //Aloco la memoria para el buffer.
    buffer->size = tamano;
    void* stream = malloc(buffer->size);
    int offset = 0;

    memcpy(stream + offset, &(contexto->pid), sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(stream + offset, &(contexto->program_counter), sizeof(uint32_t));
    offset += sizeof(uint32_t);

     //Copio los registros
    memcpy(stream + offset, contexto->registros, sizeof(t_registros));
    offset += sizeof(t_registros);

    memcpy(stream + offset, &contexto->cant_instrucciones, sizeof(uint32_t));
    offset += sizeof(uint32_t);


    //Copio el array del largo de instrucciones
    for (int i = 0; i < contexto->cant_instrucciones; i++)
    {
        memcpy(stream + offset, &(contexto->largo_instruccion[i]), sizeof(uint32_t));
        offset += sizeof(uint32_t);
    }

    //Copio las instrucciones, usando el largo del array
    for (int i = 0; i < contexto->cant_instrucciones; i++)
    {
        memcpy(stream + offset, list_get(contexto->instrucciones, i), contexto->largo_instruccion[i]); //Puede que aca haya q usar &
        offset += contexto->largo_instruccion[i];
    }

    //Copio la tabla de instrucciones 
    int tamano_tabla_segmentos = list_size(contexto->tabla_segmentos);
    memcpy(stream + offset, &(tamano_tabla_segmentos) , sizeof(int));
    offset += sizeof(int);
    for(int i = 0; i < list_size(contexto->tabla_segmentos);i++){
        Segmento* segmento = list_get(contexto->tabla_segmentos,i);
        memcpy(stream + offset, &(segmento->tamano), sizeof(int)); 
        offset += sizeof(int);
        memcpy(stream + offset, &(segmento->id), sizeof(int)); 
        offset += sizeof(int);
        memcpy(stream+offset,&(segmento->desplazamiento),sizeof(int));
        offset += sizeof(int);
    }

    //Guardo el buffer
    buffer->stream = stream;
    buffer->size = tamano;  
    return buffer;
}

t_contexto_de_ejecucion* deserializar_contexto_de_ejecucion(t_buffer* buffer){

	if (buffer->size == 0) //Prevengo el segfault devolviendo null si no hay contexto que enviar
	{
		return NULL;
	}

	t_contexto_de_ejecucion * contexto = malloc(sizeof(t_contexto_de_ejecucion));
    contexto->registros = malloc(sizeof(t_registros));
    
    
    contexto->instrucciones = list_create();
    contexto->tabla_segmentos = list_create();


    void* stream = buffer->stream;

    //0. Deserializo el PID, PROGRAM COUNTER y REGISTROS
    memcpy(&contexto->pid, stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    memcpy(&contexto->program_counter, stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    memcpy(contexto->registros, stream, sizeof(t_registros));
    stream += sizeof(t_registros);
    
    //1. Deserializo la CANTIDAD de instrucciones
    uint32_t cant_instrucciones;
    memcpy(&cant_instrucciones, stream, sizeof(uint32_t));
    memcpy(&contexto->cant_instrucciones, stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    //2. Deserializo el ARRAY de largos de instrucciones
    contexto->largo_instruccion = malloc(sizeof(uint32_t) * cant_instrucciones);

    for (int i = 0; i < cant_instrucciones; i++)
    {
        memcpy(&(contexto->largo_instruccion[i]), stream, sizeof(uint32_t));
        stream += sizeof(uint32_t);
    }

    //3. Deserializo la LISTA de instrucciones
    
    for(int i = 0; i < cant_instrucciones; i++)
    {
        char* instruccion_leida = malloc(contexto->largo_instruccion[i] * sizeof(char));
        memcpy(instruccion_leida, stream, contexto->largo_instruccion[i]);

        list_add(contexto->instrucciones, instruccion_leida);
        stream += contexto->largo_instruccion[i];

    }

    //4. Desearlizo la TABLA de segmentos
    int cantidad_segmentos;
    memcpy(&(cantidad_segmentos),stream,sizeof(int));
    stream += sizeof(int);
    for(int i = 0; i < cantidad_segmentos;i++){
        Segmento* segmento = malloc(sizeof(Segmento));
        memcpy(&(segmento->tamano),stream,sizeof(int));
        stream += sizeof(int);
        memcpy(&(segmento->id),stream,sizeof(int));
        stream += sizeof(int);
        memcpy(&(segmento->desplazamiento),stream,sizeof(int));
        stream += sizeof(int);
        list_add(contexto->tabla_segmentos,segmento);
    }
    
    return contexto;
}


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

char* leer_registro(char* registro_char, t_contexto_de_ejecucion* contexto)
{
    enum Registros registro = string_a_registro(registro_char);

    char* valor_en_registro;

    switch (registro)
    {
    case rAX:
        strcpy(valor_en_registro, contexto->registros->AX);
        break;
    case rBX:
        strcpy(valor_en_registro, contexto->registros->BX);
        break;
    case rCX:
        strcpy(valor_en_registro, contexto->registros->CX);
        break;
    case rDX:
        strcpy(valor_en_registro, contexto->registros->DX);
        break;

    // Registros 8 bits
    case rEAX:
        strcpy(valor_en_registro, contexto->registros->EAX);
        break;

    case rEBX:
        strcpy(valor_en_registro, contexto->registros->EBX);
        break;

    case rECX:
        strcpy(valor_en_registro, contexto->registros->ECX);
        break;

    case rEDX:
        strcpy(valor_en_registro, contexto->registros->EDX);
        break;

    // Registros de 16 bits
    case rRAX:
        strcpy(valor_en_registro, contexto->registros->RAX);
        break;

    case rRBX:
        strcpy(valor_en_registro, contexto->registros->RBX);
        break;

    case rRCX:
        strcpy(valor_en_registro, contexto->registros->RCX);
        break;

    case rRDX:
        strcpy(valor_en_registro, contexto->registros->RDX);
        break;
    }
    return valor_en_registro;
}

int traduccion_dir_logica_fisica(int dir_logica, t_contexto_de_ejecucion* contexto){

    int tam_max_segmento = 10; //HAY QUE SACARLO DEL CONFIG
    int num_segmento = floor(dir_logica / tam_max_segmento);
    int desplazamiento_segmento = dir_logica % tam_max_segmento;


    Segmento* segmento = list_get(contexto->tabla_segmentos, 0); //Hardcode, cambiar
    /*bool obtener_segmento(void *elemento)
    {
        Segmento *segmento = elemento;
        return segmento->id == num_segmento;
    }

    Segmento* segmento = list_find(contexto->tabla_segmentos, obtener_segmento);*/

    int dir_fisica = segmento->desplazamiento + desplazamiento_segmento;
    return dir_fisica;
}