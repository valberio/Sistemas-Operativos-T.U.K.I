#ifndef INSTRUCCIONES_H_
#define INSTRUCCIONES_H_

#include <string.h>
#include<loggers/loggers_utils.h>
#include<pcb/pcb.h>


int set(t_log* logger, char** instrucciones, t_contexto_de_ejecucion* contexto);
int yield(t_log* logger, t_contexto_de_ejecucion* contexto, int conexion_cpu_kernel);
int exit_instruccion(t_log* logger, t_contexto_de_ejecucion* contexto, int conexion_cpu_kernel);
int wait(t_log* logger, t_contexto_de_ejecucion* contexto, int conexion_cpu_kernel, char** instrucciones);
int signal_instruccion(t_log* logger, t_contexto_de_ejecucion* contexto, int conexion_cpu_kernel, char** instrucciones);
int i_o(t_log* logger, t_contexto_de_ejecucion* contexto, int conexion_cpu_kernel, char** instrucciones);

enum Registros string_a_registro(char* registro);

//La instruccion EXIT tiene una i para no pisarse con el estado de proceso EXIT
enum Instrucciones{
    SET,
    YIELD,
    EXIT,
    I_O,
    WAIT,
    SIGNAL
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