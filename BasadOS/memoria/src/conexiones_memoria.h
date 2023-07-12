#ifndef CONEXIONES_MEMORIA_H_
#define CONEXIONES_MEMORIA_H_

#include<loggers/loggers_utils.h>
#include<sockets/server_utils.h>
#include "funciones_segmentos.h"
#include<pcb/pcb.h>
#include<pcb/registros.h>
#include <pthread.h>

extern int cantidad_maxima_segmentos_por_proceso;
extern Segmento* segmento_0;
extern t_list* lista_de_memoria;
extern t_config* config;
extern t_log* logger;
extern void* espacio_de_memoria;
extern int retardo_acceso_memoria;
extern int retardo_compactacion;

typedef struct 
{
    int conexion;
}parametros_de_hilo;

void* comunicacion_con_cpu(void* );
void* comunicacion_con_kernel(void*);
void* comunicacion_con_filesystem(void* arg);
op_code respuesta_a_kernel(Segmento*,t_contexto_de_ejecucion*);
void finalizar_proceso(t_contexto_de_ejecucion* contexto_de_ejecucion);


#endif /* CONEXIONES_MEMORIA_H_ */
