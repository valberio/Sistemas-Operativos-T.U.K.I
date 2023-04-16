#include <stdlib.h>
#include <stdio.h>
#include <commons/log.h>

int main(int argc, char* argv[]) {
    puts(" wrld!!");
    t_log *logger_consola = log_create("logs/log_consola.log","log_consola",true,LOG_LEVEL_INFO);
    log_info(logger_consola, "Mensaje info de prueba");
    log_destroy(logger_consola);

    return 0;
}
