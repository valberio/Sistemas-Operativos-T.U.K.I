#ifndef PLANIFICACION_H_
#define PLANIFICACION_H_

#include<pcb/pcb.h>
#include<loggers/loggers_utils.h>
#include<commons/collections/queue.h>


t_queue* crear_cola_fifo();
void agregar_proceso_a_fifo(t_queue*, int);
int siguiente_proceso_a_ejecutar(t_queue*);

#endif /* PLANIFICACION_H_ */