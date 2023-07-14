#include "main_memoria.h"

t_log *logger;
t_config *config;
t_list *lista_de_memoria;
void *espacio_de_memoria;
int retardo_acceso_memoria;
int retardo_compactacion;

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        return EXIT_FAILURE;
    }
    // Lectura e impresion de pseudocodigo
    config = iniciar_config(argv[1]);
    logger = iniciar_logger("log_memoria.log", "MEMORIA");
    // La memoria tiene en paralelo 3 conexiones: con kernel, cpu, y fileSystem

    int tamano = config_get_int_value(config, "TAM_MEMORIA");
    int tamano_segmento_0 = config_get_int_value(config, "TAM_SEGMENTO_0");
    retardo_acceso_memoria = config_get_int_value(config, "RETARDO_MEMORIA") / 1000;
    retardo_compactacion = config_get_int_value(config, "RETARDO_COMPACTACION") / 1000;
    reservar_espacio_de_memoria(tamano, tamano_segmento_0);

    // Creo el server de la memoria en esta IP y puerto

    char *puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
    int servidor = iniciar_servidor(logger, puerto_escucha);

    // Lanzo el hilo que espera pedidos de la CPU

    int conexion_filesystem = esperar_cliente(servidor);
    if (conexion_filesystem != -1)
    {
        log_info(logger, "Se conecto el filesystem");
    }

    int conexion_cpu = esperar_cliente(servidor);

    int conexion_kernel = esperar_cliente(servidor);

    parametros_de_hilo parametros_cpu;
    parametros_cpu.conexion = conexion_cpu;

    pthread_t hilo_comunicacion_cpu;
    pthread_create(&hilo_comunicacion_cpu, NULL, comunicacion_con_cpu, (void *)&parametros_cpu);
    pthread_detach(hilo_comunicacion_cpu);

    parametros_de_hilo parametros_kernel;
    parametros_kernel.conexion = conexion_kernel;

    pthread_t hilo_comunicacion_kernel;
    pthread_create(&hilo_comunicacion_kernel, NULL, comunicacion_con_kernel, (void *)&parametros_kernel);
    pthread_detach(hilo_comunicacion_kernel);

    parametros_de_hilo parametros_filesystem;
    parametros_filesystem.conexion = conexion_filesystem;

    pthread_t hilo_comunicacion_filesystem;
    pthread_create(&hilo_comunicacion_filesystem, NULL, comunicacion_con_filesystem, (void *)&parametros_filesystem);
    pthread_join(hilo_comunicacion_filesystem, NULL);
}
