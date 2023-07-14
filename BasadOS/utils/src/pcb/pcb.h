#ifndef PCB_H_
#define PCB_H_

#include<commons/collections/list.h>
#include<math.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/time.h>
#include<sockets/client_utils.h>
#include<segmentos/segmentos.h>
#include<pcb/registros.h>




typedef struct{
    uint32_t pid;
    uint32_t program_counter;
    t_registros* registros;
    uint32_t cant_instrucciones;
    uint32_t* largo_instruccion;//Guardo la longitud de cada instruccion en este array,
                                //para más placer. Lleno los valores en otro lado. Como
                                //puedo declarar arrays de longitud variable en un 
                                //struct, lo aloco y lleno con los datos que preciso en
                                //el constructor del contexto
    t_list* instrucciones;
    t_list* tabla_segmentos;
}t_contexto_de_ejecucion;

typedef struct{
    int estado;
    uint32_t pid;
    int socket_consola;
    t_contexto_de_ejecucion* contexto_de_ejecucion;
    
    time_t tiempo_de_llegada_a_ready;
    clock_t inicio_de_uso_de_cpu;
    clock_t fin_de_uso_de_cpu;
    double estimado_rafaga;
    double tiempo_de_la_ultima_rafaga;

    t_list* tabla_archivos_abiertos;
} t_pcb;

t_contexto_de_ejecucion* crear_contexto_de_ejecucion(char* instrucciones);
t_pcb* crear_pcb(char* instrucciones, int socket, double estimado_rafaga);
t_list* string_a_lista(char* str);
void liberar_pcb(t_pcb* pcb);
void liberar_contexto_de_ejecucion(t_contexto_de_ejecucion* contexto);
void enviar_contexto_de_ejecucion(t_contexto_de_ejecucion* contexto, int conexion_socket);
t_paquete* recibir_contexto_de_ejecucion(int socket_cliente);
void instanciar_registros(t_registros* registro);
t_buffer* serializar_contexto(t_contexto_de_ejecucion* contexto);
t_contexto_de_ejecucion* deserializar_contexto_de_ejecucion(t_buffer* buffer);
t_buffer *serializar_lista_segmentos(t_list *tabla);
t_list *deserializar_lista_de_segmentos(t_buffer *buffer);
int obtener_segmento_por_id(int id, t_list* tabla_segmentos);



enum Estados {
    NEW, 
    READY, 
    RUNNING,
    BLOCKED, 
    EXITT
};

#endif  /*PCB_H_ */