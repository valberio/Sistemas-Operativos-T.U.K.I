#ifndef PCB_H_
#define PCB_H_

#include<commons/collections/list.h>
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

typedef struct{
    int program_counter;
    t_list* lista_instrucciones;
    t_registros* registros;
} t_contexto_de_ejecucion;

typedef struct{
    int estado;
    int pid;
    t_contexto_de_ejecucion contexto_de_ejecucion;
    t_list* tabla_segmentos;
    t_list* tabla_archivos_abiertos;
    int estimado_rafaga;
} t_pcb;

t_pcb *pcb_create();
t_list* agregar_instrucciones_a_pcb(char*);
void liberar_pcb(t_pcb* pcb);
void enviar_contexto_de_ejecucion(t_contexto_de_ejecucion* contexto_de_ejecucion,int);
t_contexto_de_ejecucion* recibir_contexto_de_ejecucion(int socket_cliente);
t_contexto_de_ejecucion* deserializar_contexto_de_ejecucion(t_buffer* buffer);
void liberar_contexto_de_ejecucion(t_contexto_de_ejecucion*);

/*enum Estados {
    NEW, 
    READY, 
    RUNNING,
    BLOCKED, 
    EXIT
};*/
//void liberar_registros(t_registros registros);

#endif  PCB_H_ 