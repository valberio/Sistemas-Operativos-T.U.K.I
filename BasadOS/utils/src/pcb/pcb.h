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
    uint32_t program_counter;
    t_registros* registros;
    uint32_t cant_instrucciones;
    uint32_t* largo_instruccion;//Guardo la longitud de cada instruccion en este array,
                                //para más placer. Lleno los valores en otro lado. Como
                                //puedo declarar arrays de longitud variable en un 
                                //struct, lo aloco y lleno con los datos que preciso en
                                //el constructor del contexto
    t_list* instrucciones;
}t_contexto_de_ejecucion;

typedef struct{
    int estado;
    int pid;
    t_contexto_de_ejecucion* contexto_de_ejecucion;
    t_list* tabla_segmentos;
    t_list* tabla_archivos_abiertos;
    int estimado_rafaga;
} t_pcb;

t_contexto_de_ejecucion* crear_contexto_de_ejecucion(char* instrucciones);
t_pcb* crear_pcb();
t_list* string_a_lista(char* str);
void liberar_pcb(t_pcb* pcb);
void liberar_contexto_de_ejecucion(t_contexto_de_ejecucion* contexto);
void enviar_contexto_de_ejecucion(t_contexto_de_ejecucion* contexto, int conexion_socket);
t_contexto_de_ejecucion* recibir_contexto_de_ejecucion(int socket_cliente);
void instanciar_registros(t_registros* registro);
t_buffer* serializar_contexto(t_contexto_de_ejecucion* contexto);
t_contexto_de_ejecucion* deserializar_contexto_de_ejecucion(t_buffer* buffer);



enum Estados {
    NEW, 
    READY, 
    RUNNING,
    BLOCKED, 
    EXITT
};
//void liberar_registros(t_registros registros);

#endif  /*PCB_H_ */