#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include<sockets/client_utils.h>
#include<sockets/server_utils.h>
#include<loggers/loggers_utils.h>
//#include "../../memoria/src/main.h"
#include <commons/bitarray.h>
#include <math.h>
#include <libgen.h>

typedef struct {
    unsigned block_size;
    unsigned block_count;
}t_superbloque;

typedef struct {
    uint32_t* data;
}t_bloque;

typedef struct {
    char* name;
    uint32_t size; //En bytes
    uint32_t direct_pointer; //Apunta al primer bloque de datos del archivo.
    uint32_t indirect_pointer //Apunta a un bloque que contendrá los punteros a los siguientes bloques del archivo.
}t_fcb; 

#endif /* FILESYSTEM_H_ */