#include<loggers/loggers_utils.h>

void set(char* registro, char* valor, t_log* logger)
{

    //Guardo en el registro de registros.c el valor
    log_info(logger, "Entré en la ejecución de SET");
}

void yield(t_log* logger)
{
    log_info(logger, "Entré en la ejecución de YIELD");
}

void exit_instruccion(t_log* logger)
{
    log_info(logger, "Entré en la ejecución de EXIT");
}