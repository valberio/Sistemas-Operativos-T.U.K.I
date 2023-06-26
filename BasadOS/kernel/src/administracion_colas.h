#ifndef ADMINISTRACION_COLAS_H_
#define ADMINISTRACION_COLAS_H_


#include<pcb/pcb.h>
#include<sockets/server_utils.h>
#include<planificacion.h>
#include<hilos_kernel.h>
#include<commons/collections/queue.h>
#include<commons/config.h>
#include<manejo_recursos.h>

#include<semaphore.h>
#include<stdio.h>
#include<time.h>

typedef struct{
    char* nombre_archivo;
    uint32_t posicion_puntero;
    t_queue* procesos_bloqueados;
} Archivo;

extern sem_t semaforo_procesos_en_exit;
extern sem_t semaforo_procesos_en_ready;
extern sem_t semaforo_multiprogramacion;
extern sem_t semaforo_de_procesos_para_ejecutar;
extern sem_t mutex_cola_exit;
extern sem_t mutex_cola_new;
extern sem_t mutex_cola_ready;
extern sem_t mutex_cola_blocked;


extern t_queue* cola_new;
extern t_queue* cola_ready;
extern t_queue* cola_exit;
extern t_queue* cola_blocked;

extern t_list* recursos;
extern t_list* lista_archivos_abiertos;
extern t_log* logger;
extern t_config* config;

void administrar_procesos_de_ready(int , int, int);
void* administrar_procesos_de_new_wrapper(void*);
void administrar_procesos_de_new(int );
void* administrar_procesos_de_exit();
Archivo* crear_archivo(char*);
int buscar_archivo_en_tabla_global(char*);
void aniadir_a_bloqueados(t_pcb* , char*);



#endif /* ADMINISTRACION_COLAS_H_ */