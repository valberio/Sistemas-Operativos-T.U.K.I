#include "main.h"

/*------------------------------------------------------------------*/
/*						CHECKPOINT 2								*/
/*------------------------------------------------------------------*/
/*		Levanta el archivo de configuración: HECHO!
		Espera las conexiones de CPU, Kernel y File System: HECHO!
																	*/
/*------------------------------------------------------------------*/




//Probando cómo serializar una lista.






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

    printf("El pc pre-serializado es %i", contexto->program_counter);
    printf("El pc deserializado es %i\n", con->program_counter);
    for (int i = 0; i < 4; i++)
    {
        printf("instruccion deserializada %s\n", list_get(con->instrucciones, i));
    }
    printf("Imprimo %s\n", con->registros->DX);


    t_log* logger = iniciar_logger("log_memoria.log", "Servidor");
    t_config* config = iniciar_config("configs/memoria.config");
	//La memoria tiene en paralelo 3 conexiones: con kernel, cpu, y fileSystem

	//Creo el server de la memoria en esta ip y puerto
	char* ip = config_get_string_value(config, "IP");

    char* puerto_filesystem = config_get_string_value(config, "PUERTO_FILESYSTEM");

    int servidor_memoria_filesystem = iniciar_servidor(logger, ip, puerto_filesystem);
	//Guardo las conexiones con cada modulo en un socket distinto,
	//cada módulo se conecta a través de un puerto diferente.
   
   int conexion_filesystem = esperar_cliente(servidor_memoria_filesystem);
   if (conexion_filesystem)
   {
		log_info(logger, "Se conectó el fileSystem");
        enviar_contexto(contexto, conexion_filesystem);
   }

    list_destroy(lista);

}

void instanciar_registros(t_registros* registro)
{
    strcpy(registro->AX, "0000");
	strcpy(registro->BX, "0000");
	strcpy(registro->CX, "0000");
	strcpy(registro->DX, "UNGA");
	
	strcpy(registro->EAX, "00000000");
	strcpy(registro->EBX, "00000000");
	strcpy(registro->ECX, "00000000");
	strcpy(registro->EDX, "00000000");

    strcpy(registro->RAX, "0000000000000000");
	strcpy(registro->RBX, "0000000000000000");
	strcpy(registro->RCX, "0000000000000000");
	strcpy(registro->RDX, "0000000000000000");
}

t_contexto* crear_contexto(t_list* instrucciones){
    t_contexto* contexto = malloc(sizeof(t_contexto));
    contexto->program_counter = 11;
    contexto->registros = malloc(sizeof(t_registros));

    instanciar_registros(contexto->registros);

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

    //0.1 Tamaño del PROGRAM COUNTER
    tamano += sizeof(uint32_t);

    //0.2 Tamaño de los registros
    tamano += sizeof(t_registros);

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

    memcpy(stream + offset, &(contexto->program_counter), sizeof(uint32_t));
    offset += sizeof(uint32_t);

     //Copio los registros
    memcpy(stream + offset, contexto->registros, sizeof(t_registros));
    offset += sizeof(t_registros);

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
    buffer->size = tamano;  
    return buffer;
}

t_contexto* deserilizar_contexto_(t_buffer* buffer)
{
    //Necesito:
    //La cantidad de instrucciones
    //Hago el proceso inverso: copio del stream a las variables

    t_contexto * contexto = malloc(sizeof(t_contexto));
    contexto->registros = malloc(sizeof(t_registros));  //Tengo que copiar los registros uno por uno?
    contexto->instrucciones = list_create();

    void* stream = buffer->stream;

    //0. Deserializo PROGRAM COUNTER y REGISTROS
    memcpy(&contexto->program_counter, stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    memcpy(contexto->registros, stream, sizeof(t_registros));
    stream += sizeof(t_registros);
    
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

void enviar_contexto(t_contexto* contexto, int conexion_socket)
{
    t_paquete* paquete = crear_paquete();
    t_buffer* buffer = serializar_contexto(contexto);

    paquete->buffer = buffer;
    paquete->codigo_operacion = 0;

    enviar_paquete(paquete, conexion_socket);
    eliminar_paquete(paquete);
}

t_contexto* recibir_contexto(int conexion_socket)
{
    t_paquete* paquete = crear_paquete();
    t_contexto* contexto = malloc(sizeof(t_contexto));

    recv(conexion_socket, &(paquete->codigo_operacion), sizeof(int), MSG_WAITALL);
    recv(conexion_socket, &(paquete->buffer->size), sizeof(uint32_t), MSG_WAITALL);
    paquete->buffer->stream = malloc(paquete->buffer->stream);
    recv(conexion_socket, paquete->buffer->stream, paquete->buffer->size, MSG_WAITALL);

    contexto = deserilizar_contexto_(paquete->buffer);
    eliminar_paquete(paquete);

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
