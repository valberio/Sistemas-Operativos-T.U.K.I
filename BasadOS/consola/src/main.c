#include "main.h"

int main(int argc, char* argv[]) {
 
    t_log* logger_consola = log_create("/home/utnso/tp-2023-1c-BasadOS/BasadOS/consola/logs/log_consola.log","log_consola",true,LOG_LEVEL_INFO);
    log_info(logger_consola, "Mensaje info de prueba");

    char* puerto_consola = "10577";
    char*	ip = "127.0.0.1";

    int conexion_kernel = crear_conexion_al_server(logger_consola, ip, puerto_consola);
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
