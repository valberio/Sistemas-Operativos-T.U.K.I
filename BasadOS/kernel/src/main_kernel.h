#ifndef KERNEL_H_
#define KERNEL_H_

#include<pcb/pcb.h>
#include<sockets/client_utils.h>
#include<sockets/server_utils.h>
#include<loggers/loggers_utils.h>
#include"planificacion.h"
#include <pthread.h>
#include <semaphore.h>
#include"hilos_kernel.h"
#include"administracion_colas.h"
#include"manejo_recursos.h"


extern t_queue* cola_new;
extern t_queue* cola_ready;
extern t_queue* cola_blocked;
extern t_queue* cola_exit;
extern t_list* recursos;

extern t_log* logger;
extern t_config* config;

size_t contarCadenas(char** array); 



#endif /* KERNEL_H_ */