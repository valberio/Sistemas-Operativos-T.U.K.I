#ifndef CPU_H_
#define CPU_H_

#include<sockets/client_utils.h>
#include<sockets/server_utils.h>
#include<loggers/loggers_utils.h>
#include<pcb/pcb.h>
#include "utils/ciclo_instruccion/ciclo_instruccion.h"
#include"../../memoria/src/segmentos.h"





int conectarse_a_memoria(t_config*, t_log* );
int conexion_a_kernel(t_config*,t_log* );
t_list* separar_string(char* cadena);
void liberar_array_instrucciones(char** instruccion_array);

#endif /* CPU_H_ */

