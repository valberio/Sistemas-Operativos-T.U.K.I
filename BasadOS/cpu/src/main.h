#ifndef CPU_H_
#define CPU_H_

#include<sockets/client_utils.h>
#include<sockets/server_utils.h>
#include<loggers/loggers_utils.h>
#include<sockets/socketUtils.c>
#include<pcb/pcb.h>

char* fetch(t_contexto_de_ejecucion * contexto);
char* decode(char* instruccion_string);
t_list* separar_string(char* cadena);
void execute(t_log* logger, char** instrucciones);

#endif /* CPU_H_ */

