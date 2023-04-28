#ifndef CPU_H_
#define CPU_H_

#include<sockets/client_utils.h>
#include<sockets/server_utils.h>
#include<loggers/loggers_utils.h>
#include<sockets/socketUtils.c>
#include "registros.h"

char* fetch();
enum Instrucciones decode(char* instruccion_string);
char** separar_string(char* cadena);

#endif /* CPU_H_ */

