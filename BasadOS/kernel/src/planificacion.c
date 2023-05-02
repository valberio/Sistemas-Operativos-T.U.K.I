#include "planificacion.h"

t_queue* crear_cola_fifo()
{
    return queue_create();
}

void agregar_proceso_a_fifo(t_queue* cola, int pid)
{
    queue_push(cola, pid);
}

int siguiente_proceso_a_ejecutar(t_queue* cola)
{
    int pid = queue_pop(cola);
    return pid;
}