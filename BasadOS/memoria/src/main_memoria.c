#include "main.h"

/*------------------------------------------------------------------*/
/*						CHECKPOINT 2								*/
/*------------------------------------------------------------------*/
/*		Levanta el archivo de configuración: HECHO!
		Espera las conexiones de CPU, Kernel y File System: HECHO!
																	*/
/*------------------------------------------------------------------*/


#include "pcb/pcb.h"

//Probando cómo serializar una lista.


typedef struct{

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
    uint8_t codigo_operacion;
    t_buffer* buffer;
}paquete;

t_contexto* crear_contexto(t_list* instrucciones);
t_buffer* serializar_contexto(t_contexto* contexto);
t_contexto* deserilizar_contexto_(t_buffer* buffer);

int main()
{
    t_list* lista = list_create();
    list_add(lista, "SET AX HOLA");
    list_add(lista, "YIELD");
    list_add(lista, "EXIT");
    list_add(lista, "SEXOO");

    t_contexto* contexto = crear_contexto(lista);
    
    t_buffer* buffer = serializar_contexto(contexto);
    t_contexto* con = deserilizar_contexto_(buffer);

    for (int i = 0; i < con->cant_instrucciones; i++)
    {
        printf("%s\n", list_get(con->instrucciones, i));
    }

    list_destroy(lista);

}




t_contexto* crear_contexto(t_list* instrucciones){
    t_contexto* contexto = malloc(sizeof(t_contexto));
    contexto->instrucciones = list_create();
    
    for (int i = 0; i < list_size(instrucciones); i++)
    {
        list_add(contexto->instrucciones, list_get(instrucciones, i));
    }

    contexto->cant_instrucciones = list_size(instrucciones);
    contexto->largo_instruccion = malloc(sizeof(uint32_t) * contexto->cant_instrucciones);

   
    for(int i = 0; i < contexto->cant_instrucciones; i++){
        char* instruccion = list_get(instrucciones, i);
        contexto->largo_instruccion[i] = strlen(instruccion) + 1;
    }

    return contexto;
}

t_buffer* serializar_contexto(t_contexto* contexto)
{
    t_buffer* buffer = malloc(sizeof(t_buffer));

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
    offset += sizeof(uint32_t);

    //Copio el array del largo de instrucciones
    for (int i = 0; i < contexto->cant_instrucciones; i++)
    {
        printf("%i\n", i);
        memcpy(stream + offset, &(contexto->largo_instruccion[i]), sizeof(uint32_t));
        offset += sizeof(uint32_t);
    }

    //Copio las instrucciones, usando el largo del array
    for (int i = 0; i < contexto->cant_instrucciones; i++)
    {
        memcpy(stream + offset, list_get(contexto->instrucciones, i), contexto->largo_instruccion[i]); //Puede que aca haya q usar &
        offset += contexto->largo_instruccion[i];
    }

    //Guardo el buffer
    buffer->stream = stream;
    return buffer;
}

t_contexto* deserilizar_contexto_(t_buffer* buffer)
{
    //Necesito:
    //La cantidad de instrucciones
    //Hago el proceso inverso: copio del stream a las variables

    t_contexto * contexto = malloc(sizeof(t_contexto));
    contexto->instrucciones = list_create();

    void* stream = buffer->stream;
    
    //1. Deserializo la CANTIDAD de instrucciones
    uint32_t cant_instrucciones;
    memcpy(&cant_instrucciones, stream, sizeof(uint32_t));
    memcpy(&contexto->cant_instrucciones, stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    //2. Deserializo el ARRAY de largos de instrucciones
    uint32_t* largo_instruccion = malloc(sizeof(uint32_t) * cant_instrucciones);

    for (int i = 0; i < cant_instrucciones; i++)
    {
        memcpy(&(largo_instruccion[i]), stream, sizeof(uint32_t));
        stream += sizeof(uint32_t);
    }

    //3. Deserializo la LISTA de instrucciones
    
    for(int i = 0; i < cant_instrucciones; i++)
    {
        char* instruccion_leida = malloc(largo_instruccion[i] * sizeof(char));
        memcpy(instruccion_leida, stream, largo_instruccion[i]);
        //instruccion_leida[largo_instruccion[i]] = '\0';
        list_add(contexto->instrucciones, instruccion_leida);
        stream += largo_instruccion[i];
    }
    return contexto;
}




/*
int main(int argc, char* argv[]) {

    t_log* logger = iniciar_logger("log_memoria.log", "Servidor");
    t_config* config = iniciar_config("../configs/memoria.config");
	//La memoria tiene en paralelo 3 conexiones: con kernel, cpu, y fileSystem

	//Creo el server de la memoria en esta ip y puerto
	char* ip = config_get_string_value(config, "IP");

	char* puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");
    char* puerto_cpu = config_get_string_value(config, "PUERTO_CPU");
    char* puerto_filesystem = config_get_string_value(config, "PUERTO_FILESYSTEM");

	int servidor_memoria_kernel = iniciar_servidor(logger, ip, puerto_kernel);
    int servidor_memoria_cpu = iniciar_servidor(logger, ip, puerto_cpu);
    int servidor_memoria_filesystem = iniciar_servidor(logger, ip, puerto_filesystem);
	//Guardo las conexiones con cada modulo en un socket distinto,
	//cada módulo se conecta a través de un puerto diferente.
   
   int conexion_filesystem = esperar_cliente(servidor_memoria_filesystem);
   if (conexion_filesystem)
   {
		log_info(logger, "Se conectó el fileSystem");
   }

    int conexion_cpu = esperar_cliente(servidor_memoria_cpu);
   if (conexion_cpu)
   {
		log_info(logger, "Se conectó la CPU");
   }

   int conexion_kernel = esperar_cliente(servidor_memoria_kernel);
   if (conexion_kernel)
   {
		log_info(logger, "Se conecto el kernel");
   }

   if (conexion_kernel == -1)
   {
		log_info(logger, "Error conectando el kernel");
		return 0;
   }
}
*/
/*void iterator(char* value) {
	log_info(logger,"%s", value);
}*/
