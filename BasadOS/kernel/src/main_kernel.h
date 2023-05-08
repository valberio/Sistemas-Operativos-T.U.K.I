#ifndef KERNEL_H_
#define KERNEL_H_

#include<pcb/pcb.h>
#include<sockets/client_utils.h>
#include<sockets/server_utils.h>
#include<loggers/loggers_utils.h>
#include"planificacion.h"


int conectarse_a_memoria(t_log* logger);
int conexion_a_kernel(t_log* logger);
t_pcb * crear_pcb( char* instrucciones);
void terminar_programa(t_log* logger, t_config* config);
void enviar_instrucciones_a_cpu();

typedef struct {
    int conexion_servidor;
    int conexion_cliente;
} parametros_hilo;



#endif /* KERNEL_H_ */