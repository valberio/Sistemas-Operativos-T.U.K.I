#ifndef MANEJO_RECURSOS_H_
#define MANEJO_RECURSOS_H_

#include<pcb/pcb.h>
#include<commons/collections/queue.h>
#include<administracion_colas.h>
#include <semaphore.h>

extern t_log* logger;

extern sem_t semaforo_procesos_en_exit;
extern sem_t semaforo_procesos_en_ready;

extern sem_t mutex_cola_exit;
extern sem_t mutex_cola_ready;

extern t_queue* cola_exit;
extern t_queue* cola_ready;

extern t_list* recursos;


void crear_lista_de_recursos(t_list* , char** ,char**);
bool buscar_pid(void* , int );
void manipulador_de_IO(char* , int );
int wait_recurso(char* , t_pcb* );
size_t contarCadenas(char** array); 
int signal_recurso(char* , t_pcb* );


typedef struct{
    char* recurso;
    int instancias;
    t_queue* cola_de_bloqueados;
} Recurso;

#endif /* MANEJO_RECURSOS_H_ */