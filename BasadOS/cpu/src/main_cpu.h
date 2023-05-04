#ifndef CPU_H_
#define CPU_H_

#include<sockets/client_utils.h>
#include<sockets/server_utils.h>
#include<loggers/loggers_utils.h>
#include "registros.h"
#include<pcb/pcb.h>

/*char* fetch();
char** decode(char* );
void execute(t_log* , char** );
*/
t_list* separar_string(char* );
int conectarse_a_memoria(t_config*, t_log* );
int conexion_a_kernel(t_config*,t_log* );

#endif /* CPU_H_ */

