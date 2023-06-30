#ifndef HILOS_KERNEL_H
#define HILOS_KERNEL_H

#include<pcb/pcb.h>
#include"planificacion.h"
#include<administracion_colas.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct{
    int conexion;
    char* mensaje;
    char* valor;
    double estimacion;
    int pid;
} Parametros_de_hilo;


void crear_hilo(pthread_t,int ,void*(*)(void*));
void crear_proceso(char* codigo_recibido, int socket_consola, double estimado_inicial);
void* crear_proceso_wrapper(void* );
void recibir_de_consolas(int server_consola);
void* recibir_de_consolas_wrapper(void* );
void* manipulador_de_IO_wrapper(void* );
void manipulador_de_IO(char*, int);



#endif /* HILOS_KERNEL_H_ */