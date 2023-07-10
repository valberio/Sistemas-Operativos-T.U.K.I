#ifndef INSTRUCCIONES_H_
#define INSTRUCCIONES_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include<loggers/loggers_utils.h>
#include<pcb/pcb.h>
#include<sockets/client_utils.h>
#include<sockets/server_utils.h>
#include<segmentos/segmentos.h>



int set(t_log* logger, char** instrucciones, t_contexto_de_ejecucion* contexto);
int mov_in(t_log* logger, char** instrucciones, t_contexto_de_ejecucion* contexto, int conexion_memoria_cpu);
int mov_out(t_log* logger, char** instrucciones, t_contexto_de_ejecucion* contexto, int conexion_memoria_cpu);
int f_open(t_log* logger, t_contexto_de_ejecucion* contexto, int conexion_kernel_cpu,  char** instrucciones);
int f_close(t_log* logger, char** instrucciones, t_contexto_de_ejecucion* contexto, int conexion_kernel_cpu);
int f_seek(t_log* logger, char** instrucciones, t_contexto_de_ejecucion* contexto,  int conexion_kernel_cpu);
int f_truncate(t_log* logger, char** instrucciones, t_contexto_de_ejecucion* contexto, int conexion_kernel_cpu);
int f_read(t_log* logger, char** instrucciones, t_contexto_de_ejecucion* contexto, int conexion_kernel_cpu);
int f_write(t_log* logger, char** instrucciones, t_contexto_de_ejecucion* contexto, int conexion_kernel_cpu);
int set(t_log* logger, char** instrucciones, t_contexto_de_ejecucion* contexto);
int yield(t_log* logger, t_contexto_de_ejecucion* contexto, int conexion_kernel_cpu);
int exit_instruccion(t_log* logger, t_contexto_de_ejecucion* contexto, int conexion_kernel_cpu);
int wait(t_log* logger, t_contexto_de_ejecucion* contexto, int conexion_kernel_cpu, char** instrucciones);
int signal_instruccion(t_log* logger, t_contexto_de_ejecucion* contexto, int conexion_kernel_cpu, char** instrucciones);
int i_o(t_log* logger, t_contexto_de_ejecucion* contexto, int conexion_kernel_cpu, char** instrucciones);
int create_segment(t_log* logger, char** instrucciones, t_contexto_de_ejecucion* contexto, int conexion_kernel_cpu);
int delete_segment(t_log* logger, char** instrucciones, t_contexto_de_ejecucion* contexto, int id_segmento);


//La instruccion EXIT tiene una i para no pisarse con el estado de proceso EXIT
enum Instrucciones{
    SET,
    MOV_IN,
    MOV_OUT,
    I_O,
    F_OPEN,
    F_CLOSE,
    F_SEEK,
    F_READ,
    F_WRITE,
    F_TRUNCATE,
    WAIT,
    SIGNAL,
    CREATE_SEGMENT,
    DELETE_SEGMENT,
    YIELD,
    EXIT
};




#endif /* INSTRUCCIONES_H_ */