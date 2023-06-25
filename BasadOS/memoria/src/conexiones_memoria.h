#ifndef CONEXIONES_MEMORIA_H_
#define CONEXIONES_MEMORIA_H_

#include<loggers/loggers_utils.h>
#include<sockets/server_utils.h>
#include <pthread.h>

typedef struct 
{
    int conexion;
}parametros_de_hilo;

void* comunicacion_con_cpu(void* );
void* comunicacion_con_kernel(void* arg);


#endif /* CONEXIONES_MEMORIA_H_ */
