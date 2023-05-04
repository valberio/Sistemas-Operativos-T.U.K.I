#ifndef CPU_H_
#define CPU_H_

#include<sockets/client_utils.h>
#include<sockets/server_utils.h>
#include<loggers/loggers_utils.h>
#include<pcb/pcb.h>
#include<ciclo_instruccion/ciclo_instruccion.h>
#include<instrucciones/instrucciones.h>

char* fetch(t_contexto_de_ejecucion * contexto);
char** decode(char* instruccion_string);
int execute(t_log* logger, char** instrucciones, t_registros* registro);

int conectarse_a_memoria(t_config*, t_log* );
int conexion_a_kernel(t_config*,t_log* );
t_list* separar_string(char* cadena);

#endif /* CPU_H_ */

