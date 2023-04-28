#include<loggers/loggers_utils.h>
#include "registros.h"

/*enum Registros{
    AX,
    BX,
    CX,
    DX
}*/

void set(t_log* logger, char** instrucciones)
{
    
    //Guardo en el registro de registros.c el valor
    log_info(logger, "Entré en la ejecución de SET");
    AX = instrucciones[2];
    log_info(logger, "Guardé en el registro el siguiente valor");
    log_info(logger, AX);
}

void yield(t_log* logger)
{
    log_info(logger, "Entré en la ejecución de YIELD");
}

void exit_instruccion(t_log* logger)
{
    log_info(logger, "Entré en la ejecución de EXIT");
}