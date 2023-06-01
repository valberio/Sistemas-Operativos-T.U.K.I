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

typedef struct 
{
    int conexion;
}parametros_de_hilo;


int main(int argc, char* argv[]) {

    logger = iniciar_logger("log_memoria.log", "Servidor");
    config = iniciar_config("configs/memoria.config");
	//La memoria tiene en paralelo 3 conexiones: con kernel, cpu, y fileSystem


	//Creo el server de la memoria en esta IP y puerto

	//char* IP = config_get_string_value(config,  IP");
	char* puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
    int servidor = iniciar_servidor(logger, IP, puerto_escucha);

    
     //Lanzo el hilo que espera pedidos de la CPU
    int conexion_cpu = esperar_cliente(servidor);

    int conexion_kernel = esperar_cliente(servidor);

    parametros_de_hilo parametros_cpu;
    parametros_cpu.conexion = conexion_cpu;

    pthread_t hilo_comunicacion_cpu;
    pthread_create(&hilo_comunicacion_cpu, NULL, comunicacion_con_cpu, (void*)&parametros_cpu);
    

    parametros_de_hilo parametros_kernel;
    parametros_kernel.conexion = conexion_kernel;

    pthread_t hilo_comunicacion_kernel;
    pthread_create(&hilo_comunicacion_kernel, NULL, comunicacion_con_kernel, (void*)&parametros_kernel);

    //pthread_join(hilo_comunicacion_cpu, NULL);
    pthread_join(hilo_comunicacion_kernel, NULL);
}


void* comunicacion_con_kernel(void* arg)
{
    parametros_de_hilo* parametros = (parametros_de_hilo*)arg;

    int conexion_kernel = parametros->conexion;

    if (conexion_kernel == -1) {log_info(logger, "Error con la conexión al kernel"); return NULL;}

    while(conexion_kernel >= 0)
    {
        char* mensaje = recibir_mensaje(conexion_kernel);
        log_info(logger, "Recibí de kernel: %s", mensaje);
    }

    return  NULL;   


}

void* comunicacion_con_cpu(void* arg)
{

    parametros_de_hilo* parametros = (parametros_de_hilo*)arg;

    int conexion_cpu = parametros->conexion;

    if(conexion_cpu == -1)
    {
        log_info(logger, "Error conectandose con la CPU");
        return NULL;
    } 


    while(conexion_cpu >= 0)
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

