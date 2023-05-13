/*#include "pcb/pcb.h"

//Probando cómo serializar una lista.


typedef struct{

    uint32_t cant_instrucciones;
    uint32_t* largo_instruccion;//Guardo la longitud de cada instruccion en este array,
                                //para más placer. Lleno los valores en otro lado. Como
                                //puedo declarar arrays de longitud variable en un 
                                //struct, lo aloco y lleno con los datos que preciso en
                                //el constructor del contexto
    t_list* instrucciones;
}contexto;

typedef struct{
    uint32_t size;
    void* stream;
}buffer;

typedef struct{
    uint8_t codigo_operacion;
    t_buffer* buffer;
}paquete;

contexto* crear_contexto(t_list* instrucciones);

int main()
{
    t_list* lista = list_create();
    list_add(lista, "1");
    list_add(lista, "2");

    contexto* contexto = crear_contexto(lista);
    printf("%s", list_get(contexto->cant_instrucciones, 0));

}




contexto* crear_contexto(t_list* instrucciones){
    contexto* contexto = malloc(sizeof(contexto));
    contexto->instrucciones = instrucciones;
    contexto->cant_instrucciones = list_size(instrucciones);

    contexto->largo_instruccion = malloc(sizeof(uint32_t) * contexto->cant_instrucciones);

    for(int i = 0; i < contexto->cant_instrucciones; i++){
        contexto->largo_instruccion[i] = 1;
    }

    return contexto;
}

void serializar_contexto(contexto* contexto)
{
    t_buffer* buffer = malloc(sizeof(buffer));

    //Calculo el tamaño que necesito darle al buffer.
    uint32_t tamano = 0;

    //1. Tamaño de las INSTRUCCIONES de la lista
    for (int i = 0; i < contexto->cant_instrucciones; i++)
    {
        tamano += strlen(list_get(contexto->instrucciones, i)) + 1;
    }

    //2. NO tengo que guardar el puntero de la lista, creo la lista y le guardo
    //las instrucciones en el momento de deserializar.

    //3. Tamaño del CANTIDAD INSTRUCCIONES
    tamano += sizeof(uint32_t);

    //4. Tamaño del ARRAY del largo de cada instruccion
    //Sé que el array de largos va a tener la misma cant. de elemntos que cant_instrucciones
    for(int i = 0; i < contexto->cant_instrucciones; i++)
    {
        tamano += sizeof(uint32_t);
    }
    
    //Aloco la memoria para el buffer.
    buffer->size = tamano;
    void* stream = malloc(buffer->size);
    int offset = 0;

    memcpy(stream + offset, &contexto->cant_instrucciones, sizeof(uint32_t));
    offset += 0;

    //Copio el array del largo de instrucciones
    for (int i = 0; i < contexto->cant_instrucciones; i++)
    {
        memcpy(stream + offset, contexto->largo_instruccion[i], sizeof(uint32_t));
    }
    
}







void deserializar_contexto(t_buffer* buffer)
{

}*/
