#ifndef REGISTROS_H_
#define REGISTROS_H_

#include<commons/collections/list.h>
#include<math.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/time.h>
#include<sockets/client_utils.h>

typedef struct{
    char AX[5];
    char BX[5];
    char CX[5];
    char DX[5];

    char EAX[9];
    char EBX[9];
    char ECX[9];
    char EDX[9];

    char RAX[17];
    char RBX[17];
    char RCX[17];
    char RDX[17];
}t_registros;

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

enum Registros string_a_registro(char *registro);
char* leer_registro(char *registro_char, t_registros* registros);
int tamanio_del_registro(char *registro_char);


#endif  /*REGISTROS_H_ */