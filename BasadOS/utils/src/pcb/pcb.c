#include "pcb.h"

int contador = 0;

t_pcb * crear_pcb( char* instrucciones)
{
    t_pcb *pcb = malloc(sizeof(t_pcb));
	pcb->estado = 1;
	pcb->pid = contador;
	pcb->contexto_de_ejecucion.program_counter = 0;
	pcb->contexto_de_ejecucion.registros = malloc(sizeof(t_registros));
	pcb->contexto_de_ejecucion.lista_instrucciones = list_create();
	pcb->tabla_archivos_abiertos = list_create();
	pcb->tabla_segmentos = list_create();

	contador++;

	strcpy(pcb->contexto_de_ejecucion.registros->AX, "0000");
	strcpy(pcb->contexto_de_ejecucion.registros->BX, "0000");
	strcpy(pcb->contexto_de_ejecucion.registros->CX, "0000");
	strcpy(pcb->contexto_de_ejecucion.registros->DX, "0000");
	
	strcpy(pcb->contexto_de_ejecucion.registros->EAX, "00000000");
	strcpy(pcb->contexto_de_ejecucion.registros->EBX, "00000000");
	strcpy(pcb->contexto_de_ejecucion.registros->ECX, "00000000");
	strcpy(pcb->contexto_de_ejecucion.registros->EDX, "00000000");

    strcpy(pcb->contexto_de_ejecucion.registros->RAX, "0000000000000000");
	strcpy(pcb->contexto_de_ejecucion.registros->RBX, "0000000000000000");
	strcpy(pcb->contexto_de_ejecucion.registros->RCX, "0000000000000000");
	strcpy(pcb->contexto_de_ejecucion.registros->RDX, "0000000000000000");
	
	t_list* temp_list = agregar_instrucciones_a_pcb(instrucciones);
	list_add_all(pcb->contexto_de_ejecucion.lista_instrucciones, temp_list);
	list_destroy_and_destroy_elements(temp_list, free);
	pcb->estimado_rafaga = 1;

	return pcb;
}

t_list* agregar_instrucciones_a_pcb(char* str)
{
	t_list* temp_list = list_create();
	char* buffer = malloc(sizeof(str)+1);
	strcpy(buffer,str);
	char* token = strtok(str, "\n");

	while (token != NULL) {
        //printf("%s\n", token);
		list_add(temp_list, token);
        token = strtok(NULL, "\n");
    }
	return temp_list;
}

void liberar_pcb(t_pcb* pcb)
{
	list_destroy_and_destroy_elements(pcb->contexto_de_ejecucion.lista_instrucciones, free);
	list_destroy_and_destroy_elements(pcb->tabla_segmentos, free);
	list_destroy_and_destroy_elements(pcb->tabla_archivos_abiertos, free);
	free(pcb->contexto_de_ejecucion.registros);
	free(pcb);
}

void enviar_contexto_de_ejecucion(t_contexto_de_ejecucion* contexto_de_ejecucion,int socket_cliente){
	
	char* instruccion = list_get(contexto_de_ejecucion->lista_instrucciones,0);
	
	int instruccion_longitud = strlen(instruccion);
	t_paquete* paquete = crear_paquete();
	paquete->buffer->size = sizeof(int) + sizeof(t_registros) + sizeof(int) + strlen(instruccion) ;
	void* stream = malloc(paquete->buffer->size);
	int offset = 0;

	memcpy(stream + offset, &(contexto_de_ejecucion->program_counter), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, contexto_de_ejecucion->registros, sizeof(t_registros));
	offset += sizeof(t_registros);
	memcpy(stream + offset, &instruccion_longitud, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, instruccion, strlen(instruccion));

	paquete->buffer->stream = stream;


	enviar_paquete(paquete,socket_cliente);
	eliminar_paquete(paquete);
}

t_contexto_de_ejecucion* recibir_contexto_de_ejecucion(int socket_cliente){
	t_paquete* paquete = crear_paquete();
	t_contexto_de_ejecucion* contexto_de_ejecucion = malloc(sizeof(t_contexto_de_ejecucion));
	contexto_de_ejecucion->registros = malloc(sizeof(t_registros));
	contexto_de_ejecucion->lista_instrucciones = list_create();

	recv(socket_cliente, &(paquete->codigo_operacion), sizeof(int), 0);
	recv(socket_cliente, &(paquete->buffer->size), sizeof(int), 0);
	printf("el tamanio del buffer es %d\n\n",paquete->buffer->size);
	paquete->buffer->stream = malloc(paquete->buffer->size);
	recv(socket_cliente, paquete->buffer->stream, paquete->buffer->size, 0);

	contexto_de_ejecucion = deserializar_contexto_de_ejecucion(paquete->buffer);
	eliminar_paquete(paquete);
	
	return contexto_de_ejecucion;
	
}
t_contexto_de_ejecucion* deserializar_contexto_de_ejecucion(t_buffer* buffer){
	int instruccion_longitud;
	t_contexto_de_ejecucion* contexto_de_ejecucion = malloc(sizeof(t_contexto_de_ejecucion));
	contexto_de_ejecucion->registros = malloc(sizeof(t_registros));

	contexto_de_ejecucion ->lista_instrucciones = list_create();

	void* stream = buffer->stream;
    memcpy(&(contexto_de_ejecucion->program_counter), stream, sizeof(int));
    stream += sizeof(int);
    memcpy(contexto_de_ejecucion->registros, stream, sizeof(t_registros));
    stream += sizeof(t_registros);

    // Por último, para obtener el nombre, primero recibimos el tamaño y luego el texto en sí:
    memcpy(&(instruccion_longitud), stream, sizeof(int));
    stream += sizeof(int);
    char* instruccion = malloc(instruccion_longitud);
    memcpy(instruccion, stream, instruccion_longitud);

	list_add(contexto_de_ejecucion->lista_instrucciones,instruccion);
	free(instruccion);

    return contexto_de_ejecucion;
}

// NO hace falta liberar los registros (dejo la funcion por las dudas)
// void liberar_registros(t_registros registros){
// 	free(registros.AX);
// 	free(registros.BX);
// 	free(registros.CX);
// 	free(registros.DX);

// 	free(registros.EAX);
// 	free(registros.EBX);
// 	free(registros.ECX);
// 	free(registros.EDX);

// 	free(registros.RAX);
// 	free(registros.RBX);
// 	free(registros.RCX);
// 	free(registros.RDX);
// }