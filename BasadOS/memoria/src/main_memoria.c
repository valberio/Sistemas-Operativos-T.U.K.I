#include "main_memoria.h"

/*------------------------------------------------------------------*/
/*						CHECKPOINT 2								*/
/*------------------------------------------------------------------*/
/*		Espera las peticiones de los demás módulos y responde
        con mensajes genéricos.
																	*/
/*------------------------------------------------------------------*/

t_log* logger;
t_config* config;

int main(int argc, char* argv[]) {

    logger = iniciar_logger("log_memoria.log", "Servidor");
    config = iniciar_config("configs/memoria.config");
	//La memoria tiene en paralelo 3 conexiones: con kernel, cpu, y fileSystem


	//Creo el server de la memoria en esta IP y puerto

	//char* IP = config_get_string_value(config,  IP");
	char* puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
    int servidor = iniciar_servidor(logger, IP, puerto_escucha);

    int conexion_cpu = esperar_cliente(servidor);
	
    
    //int servidor_memoria_filesystem = iniciar_servidor(logger, IP, puerto_escucha);
	//Guardo las conexiones con cada modulo en un socket distinto,
	//cada módulo se conecta a través de un puerto diferente.
   
   /*int conexion_filesystem = esperar_cliente(servidor_memoria_filesystem);
   if (conexion_filesystem)
   {
		log_info(logger, "Se conectó el fileSystem");
   }*/

    
     //Lanzo el hilo que espera pedidos de la CPU
    pthread_t hilo_comunicacion_cpu;
    pthread_create(&hilo_comunicacion_cpu, NULL, comunicacion_con_cpu, NULL);
    

   int conexion_kernel = esperar_cliente(servidor);
   if (conexion_kernel)
   {
		log_info(logger, "Se conecto el kernel");
        //Lanzo el hilo que espera pedidos del kernel
   }

   if (conexion_kernel == -1)
   {
		log_info(logger, "Error conectando el kernel");
		return 0;
   }
}


void* comunicacion_con_cpu()
{
   while(conexion_cpu)
    {
        t_paquete* peticion = recibir_paquete(conexion_cpu);
        t_paquete* paquete_respuesta = crear_paquete();

        log_info(logger, "MEMORIA recibió una petición del CPU");

        switch(peticion->codigo_operacion)
        {
            case 0: //Caso lectura
                
                paquete_respuesta->codigo_operacion = 0; 
                enviar_paquete(paquete_respuesta, conexion_cpu);
                log_info(logger, "MEMORIA respondió una petición de lectura del CPU");
                break;
            case 1: //Caso escritura
                paquete_respuesta->codigo_operacion = 1;
                enviar_paquete(paquete_respuesta, conexion_cpu);
                log_info(logger, "MEMORIA respondió una petición de escritura del CPU");
                break;
        }
    }
    return NULL;
}