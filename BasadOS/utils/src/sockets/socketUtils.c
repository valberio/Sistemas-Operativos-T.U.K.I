#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/log.h>
#include <string.h>
#include <stdlib.h>

//Inicio un server escuchando en una ip y un puerto
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



int esperar_cliente(t_log* logger, int socket_servidor)
{
	int socket_cliente = accept(socket_servidor, NULL, NULL);
	log_info(logger, "Se conecto un cliente al servidor!");
	return socket_cliente;
}



int crear_conexion_al_server(t_log* logger, char* ip, char* puerto)
{
    struct addrinfo hints;
    struct addrinfo *server_info;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    getaddrinfo(ip, puerto, &hints, &server_info);

    int socket_cliente = socket(server_info->ai_family,
                    server_info->ai_socktype,
                    server_info->ai_protocol);

    freeaddrinfo(server_info);

    if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
    {
        log_info(logger, "El cliente no se pudo conectar al server");
        return 0;
    }
    log_info(logger, "Cliente conectado al server");
    return socket_cliente;
}

void liberar_conexion(int* socket_cliente) {
    close(*socket_cliente);
    *socket_cliente = -1;
}