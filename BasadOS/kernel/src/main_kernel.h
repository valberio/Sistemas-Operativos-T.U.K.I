#ifndef KERNEL_H_
#define KERNEL_H_

#include<pcb/pcb.h>
#include<sockets/client_utils.h>
#include<sockets/server_utils.h>
#include<loggers/loggers_utils.h>
#include"planificacion.h"
#include <pthread.h>
#include <semaphore.h>

extern t_queue* cola_new;
extern t_queue* cola_ready;
extern t_queue* cola_blocked;
extern t_queue* cola_exit;

extern t_log* logger;

int conectarse_a_memoria(t_log* logger);
int conexion_a_kernel(t_log* logger);
t_pcb * crear_pcb( char* instrucciones);
void terminar_programa(t_log* logger, t_config* config);
void recibir_de_consolas(int server_consola);
void administrar_procesos_de_ready(int cliente_cpu);
void *traductor_de_parametros(void *arg);
typedef struct {
    int conexion;
} Parametros_de_hilo;



#endif /* KERNEL_H_ */