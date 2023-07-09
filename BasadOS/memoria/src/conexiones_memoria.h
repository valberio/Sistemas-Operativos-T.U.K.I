#ifndef CONEXIONES_MEMORIA_H_
#define CONEXIONES_MEMORIA_H_

#include<loggers/loggers_utils.h>
#include<sockets/server_utils.h>
#include "segmentos.h"
#include<pcb/pcb.h>
#include <pthread.h>

extern int cantidad_maxima_segmentos_por_proceso;
extern Segmento* segmento_0;


typedef struct 
{
    int conexion;
}parametros_de_hilo;

void* comunicacion_con_cpu(void* );
void* comunicacion_con_kernel(void*);
op_code respuesta_a_kernel(Segmento*,t_contexto_de_ejecucion*);

void guardar_en_registros(char* registro, char* datos, t_contexto_de_ejecucion* contexto);
void finalizar_proceso(t_contexto_de_ejecucion* contexto_de_ejecucion);


#endif /* CONEXIONES_MEMORIA_H_ */
