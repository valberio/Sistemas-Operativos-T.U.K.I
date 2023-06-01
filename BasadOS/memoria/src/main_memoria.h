#ifndef MEMORIA_H_
#define MEMORIA_H_

#include<loggers/loggers_utils.h>
#include<sockets/server_utils.h>
#include<sockets/client_utils.h>
#include "pcb/pcb.h"
#include <pthread.h>
#include <semaphore.h>

#define IP "127.0.0.1"
void* comunicacion_con_cpu(void* );
void* comunicacion_con_kernel(void* arg);


#endif /* MEMORIA_H_ */
