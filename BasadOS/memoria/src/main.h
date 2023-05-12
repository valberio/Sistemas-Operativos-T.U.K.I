#ifndef MEMORIA_H_
#define MEMORIA_H_

#include<loggers/loggers_utils.h>
#include<sockets/server_utils.h>
#include "pcb/pcb.h"


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
}t_contexto;

typedef struct{
    uint32_t size;
    void* stream;
}buffer;

typedef struct{
    int codigo_operacion;
    t_buffer* buffer;
}paquete;

t_contexto* crear_contexto(t_list* instrucciones);
t_buffer* serializar_contexto(t_contexto* contexto);
t_contexto* deserilizar_contexto_(t_buffer* buffer);
void enviar_contexto(t_contexto* contexto, int conexion_socket);
t_contexto* recibir_contexto(int conexion_socket);
void instanciar_registros(t_registros* registro);


#endif /* MEMORIA_H_ */
