#ifndef PCB_H_
#define PCB_H_

#include<commons/collections/list.h>
#include <stdlib.h>
#include<sys/time.h>

typedef struct{

    char AX[4];
    char BX[4];
    char CX[4];
    char DX[4];

    char EAX[8];
    char EBX[8];
    char ECX[8];
    char EDX[8];

    char RAX[16];
    char RBX[16];
    char RCX[16];
    char RDX[16];
}t_registros;

typedef struct{
    int program_counter;
    t_list * lista_instrucciones;
    t_registros registros;
} t_contexto_de_ejecucion;

typedef struct{
    int estado;
    int pid;
    t_contexto_de_ejecucion contexto_de_ejecucion;   
    t_list *tabla_segmentos;
    t_list *tabla_archivos_abiertos;
    int estimado_rafaga;
} t_pcb;

t_pcb *pcb_create();

#endif /* PCB_H_ */