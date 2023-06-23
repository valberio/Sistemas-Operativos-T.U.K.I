#ifndef INSTRUCCIONES_H_
#define INSTRUCCIONES_H_

#include <string.h>
#include <math.h>
#include<loggers/loggers_utils.h>
#include<pcb/pcb.h>
#include<sockets/client_utils.h>
#include<sockets/server_utils.h>
#include"../../../../memoria/src/segmentos.h"



int set(t_log* logger, char** instrucciones, t_contexto_de_ejecucion* contexto);
int mov_in(t_log* logger, char** instrucciones, t_contexto_de_ejecucion* contexto, int conexion_memoria_cpu);
int mov_out(t_log* logger, char** instrucciones, t_contexto_de_ejecucion* contexto, int conexion_memoria_cpu);
int f_open(t_log* logger, char** instrucciones, t_contexto_de_ejecucion* contexto, char* nombre_archivo);
int f_close(t_log* logger, char** instrucciones, t_contexto_de_ejecucion* contexto, char* nombre_archivo);
int f_seek(t_log* logger, char** instrucciones, t_contexto_de_ejecucion* contexto, char* nombre_archivo, int posicion);
int f_read(t_log* logger, char** instrucciones, t_contexto_de_ejecucion* contexto, char* nombre_archivo, int dir_log, int cant_bytes);
int f_write(t_log* logger, char** instrucciones, t_contexto_de_ejecucion* contexto, char* nombre_archivo, int dir_log, int cant_bytes);
int set(t_log* logger, char** instrucciones, t_contexto_de_ejecucion* contexto);
int yield(t_log* logger, t_contexto_de_ejecucion* contexto, int conexion_cpu_kernel);
int exit_instruccion(t_log* logger, t_contexto_de_ejecucion* contexto, int conexion_cpu_kernel);
int wait(t_log* logger, t_contexto_de_ejecucion* contexto, int conexion_cpu_kernel, char** instrucciones);
int signal_instruccion(t_log* logger, t_contexto_de_ejecucion* contexto, int conexion_cpu_kernel, char** instrucciones);
int i_o(t_log* logger, t_contexto_de_ejecucion* contexto, int conexion_cpu_kernel, char** instrucciones);
int create_segment(t_log* logger, char** instrucciones, t_contexto_de_ejecucion* contexto, int conexion_kernel_cpu);
int delete_segment(t_log* logger, char** instrucciones, t_contexto_de_ejecucion* contexto, int id_segmento);
void* traduccion_dir_logica_fisica(int dir_logica, t_contexto_de_ejecucion* contexto);
enum Registros string_a_registro(char* registro);

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

enum Registros{
    rAX,
    rBX,
    rCX,
    rDX,

    rEAX,
    rEBX,
    rECX,
    rEDX,

    rRAX,
    rRBX,
    rRCX,
    rRDX
};


#endif /* INSTRUCCIONES_H_ */