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

t_list* lista_de_memoria;


void* espacio_de_memoria;

//Levantar algoritmo de config
//Conectar con kernel
//Conectar con cpu
//Serializar lista de segmentos

int main(int argc, char* argv[]) {

    logger = iniciar_logger("log_memoria.log", "Servidor");
    config = iniciar_config("configs/memoria.config");
	//La memoria tiene en paralelo 3 conexiones: con kernel, cpu, y fileSystem

    int tamano = config_get_int_value(config,"TAM_MEMORIA");
    int tamano_segmento_0 = config_get_int_value(config,"TAM_SEGMENTO_0");
    
    reservar_espacio_de_memoria(tamano,tamano_segmento_0);

	//Creo el server de la memoria en esta IP y puerto

	char* puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
    int servidor = iniciar_servidor(logger, puerto_escucha);

    
     //Lanzo el hilo que espera pedidos de la CPU

    /*int conexion_filesystem = esperar_cliente(servidor);
    if(conexion_filesystem != -1){
        log_info(logger, "Se conecto el filesystem");
    }*/

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

