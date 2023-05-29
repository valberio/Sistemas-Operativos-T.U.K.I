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
extern t_list* recursos;

extern t_log* logger;
extern t_config* config;

int conectarse_a_memoria(t_log*);
int conexion_a_kernel(t_log* );
void terminar_programa(t_log* , t_config* );
void recibir_de_consolas(int );
void* recibir_de_consolas_wrapper(void* );
void crear_proceso(char*, int, double);
void* crear_proceso_wrapper(void* );
void administrar_procesos_de_ready(int );
void administrar_procesos_de_new(int );
void* administrar_procesos_de_new_wrapper(void* );
void* administrar_procesos_de_exit();
bool buscar_pid(void* , int );
void manipulador_de_IO(char* , int );
int wait_recurso(char* , t_pcb* );
int signal_recurso(char* , t_pcb* );
void* manipulador_de_IO_wrapper(void* );
size_t contarCadenas(char** array); 

t_pcb* salida_FIFO();
t_pcb* salida_HRRN();
typedef struct {
    int conexion;
    char* mensaje;
    double estimacion;
} Parametros_de_hilo;

typedef struct{
    char* recurso;
    int instancias;
    t_queue* cola_de_bloqueados;
} Recurso;

#endif /* KERNEL_H_ */