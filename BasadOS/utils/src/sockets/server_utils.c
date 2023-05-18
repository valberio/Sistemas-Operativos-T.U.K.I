#include"server_utils.h"

int iniciar_servidor(t_log* logger, char* ip, char* puerto)
{
    int socket_servidor;

    struct addrinfo hints, *servinfo;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ip, puerto, &hints, &servinfo);

    socket_servidor = socket(servinfo->ai_family,
                         servinfo->ai_socktype,
                         servinfo->ai_protocol);

    if(bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
    {
        log_info(logger, "El server no se pudo bindear al puerto\n");
    }

    listen(socket_servidor, SOMAXCONN);

    freeaddrinfo(servinfo);

    log_info(logger, "Servidor listo para recibir un cliente");

    return socket_servidor;
}



int esperar_cliente(int socket_servidor)
{
	int socket_cliente = accept(socket_servidor, NULL, NULL);
	return socket_cliente;
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), 0);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, 0);

	return buffer;
}

char* recibir_mensaje(int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));
	recv(socket_cliente, &(paquete->codigo_operacion), sizeof(int), MSG_WAITALL);
	recv(socket_cliente, &(paquete->buffer->size), sizeof(int), MSG_WAITALL);
	paquete->buffer->stream = malloc(paquete->buffer->size);
	recv(socket_cliente, paquete->buffer->stream,paquete->buffer->size, MSG_WAITALL);
	char* datos = malloc(paquete->buffer->size);
	memcpy(datos, paquete->buffer->stream,paquete->buffer->size);
	//printf("Consola me mando %s", datos);
	eliminar_paquete(paquete);

	return datos;
}

t_paquete* recibir_paquete(int conexion_socket)
{	
  	t_paquete* paquete = crear_paquete();
	recv(conexion_socket, &(paquete->codigo_operacion), sizeof(int), MSG_WAITALL);
    recv(conexion_socket, &(paquete->buffer->size), sizeof(uint32_t), MSG_WAITALL);

    paquete->buffer->stream = malloc(paquete->buffer->size);
    if (paquete->buffer->size > 0) {
    recv(conexion_socket, paquete->buffer->stream, paquete->buffer->size, MSG_WAITALL);
	}
    return paquete;
}