#ifndef CONEXIONES_MEMORIA_H_
#define CONEXIONES_MEMORIA_H_

#include<loggers/loggers_utils.h>
#include<sockets/server_utils.h>
#include "segmentos.h"
#include"../../utils/src/pcb/pcb.h"
#include <pthread.h>

typedef struct 
{
    int conexion;
}parametros_de_hilo;

void* comunicacion_con_cpu(void* );
void* comunicacion_con_kernel(void*);
op_code respuesta_a_kernel(Segmento*,t_contexto_de_ejecucion*);
int tamanio_del_registro(char* registro); //Probablemente deba ir en pcb.c
void guardar_en_registros(char* registro, char* datos, t_contexto_de_ejecucion* contexto);


#endif /* CONEXIONES_MEMORIA_H_ */
