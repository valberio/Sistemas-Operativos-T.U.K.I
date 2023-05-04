#ifndef INSTRUCCIONES_H_
#define INSTRUCCIONES_H_

#include <string.h>
#include<loggers/loggers_utils.h>
#include<pcb/pcb.h>


void set(t_log* logger, char** instrucciones, t_registros * registro);
void yield(t_log* logger);
void exit_instruccion(t_log* logger);
enum Registros string_a_registro(char* registro);

enum Registros{
    rAX,
    rBX,
    rCX,
    rDX,
};



// enum Registros string_a_registro(char* registro);
#endif /* INSTRUCCIONES_H_ */