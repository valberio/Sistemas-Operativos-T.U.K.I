#include "pcb.h"

int contador = 0;

t_pcb * crear_pcb( char* instrucciones)
{
    t_pcb *pcb = malloc(sizeof(t_pcb));
	pcb->estado = 1;
	pcb->pid = contador;
    pcb->contexto_de_ejecucion = crear_contexto_de_ejecucion(instrucciones);
	pcb->tabla_archivos_abiertos = list_create();
	pcb->tabla_segmentos = list_create();


	pcb->estimado_rafaga = 1;
	
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

    printf("%s\n", list_get(lista_instrucciones, 0));
    printf("%s\n", list_get(contexto->instrucciones, 0));

    contexto->cant_instrucciones = list_size(contexto->instrucciones); //Funciona

    contexto->largo_instruccion = malloc(sizeof(uint32_t) * contexto->cant_instrucciones);

    //Lleno el array de largos de instruccion con los datos
    for (int i = 0; i < contexto->cant_instrucciones; i++)
    {   
        contexto->largo_instruccion[i] = strlen(list_get(contexto->instrucciones, i)) + 1;
    }

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

t_contexto_de_ejecucion* recibir_contexto_de_ejecucion(int conexion_socket){
    t_paquete* paquete = crear_paquete();
    t_contexto_de_ejecucion* contexto = malloc(sizeof(t_contexto_de_ejecucion));

    recv(conexion_socket, &(paquete->codigo_operacion), sizeof(int), MSG_WAITALL);
    recv(conexion_socket, &(paquete->buffer->size), sizeof(uint32_t), MSG_WAITALL);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    recv(conexion_socket, paquete->buffer->stream, paquete->buffer->size, MSG_WAITALL);

    contexto = deserializar_contexto_de_ejecucion(paquete->buffer);
    eliminar_paquete(paquete);

    if(contexto == NULL)
    {
        printf("El buffer es nulo\n");
    }

    return contexto;
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
    
    //Aloco la memoria para el buffer.
    buffer->size = tamano;
    void* stream = malloc(buffer->size);
    int offset = 0;

    memcpy(stream + offset, &(contexto->program_counter), sizeof(uint32_t));
    offset += sizeof(uint32_t);

     //Copio los registros
    memcpy(stream + offset, contexto->registros, sizeof(t_registros));
    offset += sizeof(t_registros);

    memcpy(stream + offset, &contexto->cant_instrucciones, sizeof(uint32_t));
    offset += sizeof(uint32_t);


    //Copio el array del largo de instrucciones
    for(int i = 0; i < contexto->cant_instrucciones; i++)
    {
        printf("Largo instruccion %i\n", contexto->largo_instruccion[i]);
    }

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
    contexto->largo_instruccion =  
    contexto->instrucciones = list_create();

    void* stream = buffer->stream;

    //0. Deserializo PROGRAM COUNTER y REGISTROS
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
    return contexto;
}
