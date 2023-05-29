#ifndef CICLO_INSTRUCCION_H_
#define CICLO_INSTRUCCION_H_

#include<sockets/client_utils.h>
#include<sockets/server_utils.h>
#include<loggers/loggers_utils.h>
#include<pcb/pcb.h>
#include "../instrucciones/instrucciones.h"

char* fetch(t_contexto_de_ejecucion * contexto);
char** decode(char* instruccion_string, int retardo_instruccion);
int execute(t_log* logger, char** instrucciones, t_contexto_de_ejecucion* contexto, int conexion_cpu_kernel);
enum Instrucciones string_a_instruccion(char* instruccion);

#endif /* CICLO_INSTRUCCION_H_ */