#ifndef PLANIFICACION_H_
#define PLANIFICACION_H_

#include<pcb/pcb.h>
#include<commons/config.h>
#include<commons/collections/queue.h>
#include<time.h>
#include<semaphore.h>


extern sem_t semaforo_procesos_en_ready;
extern sem_t mutex_cola_ready;
extern sem_t mutex_cola_blocked;


extern t_queue* cola_blocked;
extern t_queue* cola_ready;

extern t_log* logger;
extern t_config* config;




t_pcb* salida_FIFO();
t_pcb* salida_HRRN();
void calcular_estimado_de_rafaga(t_pcb*);
bool el_mayor_hrr_entre(t_pcb* , t_pcb* , time_t );
char *convertir_a_char(uint32_t numero);


/*

t_queue* crear_cola_fifo();
void agregar_proceso_a_fifo(t_queue*, int);
int siguiente_proceso_a_ejecutar(t_queue*);
*/
#endif /* PLANIFICACION_H_ */