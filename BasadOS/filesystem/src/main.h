#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include<sockets/client_utils.h>
#include<sockets/server_utils.h>
#include<loggers/loggers_utils.h>
#include "../../memoria/src/main.h"
#include <commons/bitarray.h>
#include <math.h>

typedef struct {
    unsigned block_size;
    unsigned block_count;
}t_superbloque;

typedef struct {

}t_bloque;

typedef struct {
    char* name;
    uint32_t size;
    uint32_t* direct_pointer;
    uint32_t* indirect_pointer
}t_fcb; 

#endif /* FILESYSTEM_H_ */