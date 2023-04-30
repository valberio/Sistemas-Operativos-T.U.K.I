#ifndef KERNEL_H_
#define KERNEL_H_

#include<pcb/pcb.h>
#include<sockets/client_utils.h>
#include<sockets/server_utils.h>
#include<loggers/loggers_utils.h>


int conectarse_a_memoria(t_log* logger);
int conexion_a_kernel(t_log* logger);
t_pcb * crear_pcb( char* instrucciones);

#endif /* KERNEL_H_ */