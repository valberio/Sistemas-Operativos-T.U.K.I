#ifndef CONSOLA_H_
#define CONSOLA_H_

#include<sockets/client_utils.h>
#include<sockets/server_utils.h>
#include<loggers/loggers_utils.h>


int levantar_conexion(t_log*, char*,char*);
char *leer_Pseudocodigo(t_log* ,char *);

#endif /* CONSOLA_H_ */