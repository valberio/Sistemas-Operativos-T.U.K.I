#ifndef SERVER_UTILS_H_
#define SERVER_UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/config.h>
#include<commons/collections/list.h>
#include<string.h>
#include<assert.h>
#include<sockets/client_utils.h>

extern t_log* logger;


void* recibir_buffer(int*, int);

int iniciar_servidor(t_log* , char* , char* );
int esperar_cliente(int);
t_paquete* recibir_paquete(int);
char* recibir_mensaje(int);
int recibir_operacion(int);
void iterator(char*);
void terminar_programa(t_log*, t_config* );

#endif /* SERVER_UTILS_H_ */