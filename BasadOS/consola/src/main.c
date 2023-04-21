#include "main.h"

int main(int argc, char* argv[]) {
 
    t_log* logger_consola = iniciar_logger("log_consola.log","LOG_CONSOLA");
    t_config* config = iniciar_config("configs/consola.config");
    
    char* ip = config_get_string_value(config, "IP");
    char* puerto_kernel_consola = config_get_string_value(config, "PUERTO_KERNEL");
    

    int conexion_kernel = crear_conexion_al_server(logger_consola, ip, puerto_kernel_consola);
    if (conexion_kernel)
    {
        log_info(logger_consola, "Consola envió su conexión al kernel");
    }
    if ((conexion_kernel == -1))
    {
        log_info(logger_consola, "Error conectando la consola con el kernel");
    }
    return 0;
}
