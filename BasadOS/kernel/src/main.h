#ifndef KERNEL_H_
#define KERNEL_H_

#include<commons/string.h>
#include<commons/config.h>

#include <sockets/socketUtils.c>


t_log* iniciar_logger(void);
t_config* iniciar_config(void);
void leer_consola(t_log*);
void paquete(int);
void terminar_programa(int, t_log*, t_config*);

#endif /* KERNEL_H_ */