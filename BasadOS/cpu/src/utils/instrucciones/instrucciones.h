#ifndef INSTRUCCIONES_H_
#define INSTRUCCIONES_H_

#include <string.h>
#include<loggers/loggers_utils.h>
#include<pcb/pcb.h>


void set(t_log* logger, char** instrucciones, t_registros * registro);
void yield(t_log* logger);
void exit_instruccion(t_log* logger);
enum Registros string_a_registro(char* registro);

//La instruccion EXIT tiene una i para no pisarse con el estado de proceso EXIT
enum Instrucciones{
    SET,
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