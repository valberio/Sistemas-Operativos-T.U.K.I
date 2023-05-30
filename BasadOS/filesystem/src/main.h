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

Nombre del archivo: Este será el identificador del archivo, ya que no tendremos 2 archivos con el mismo nombre.
Tamaño del archivo: Indica el tamaño del archivo expresado en bytes.
Puntero directo: Apunta al primer bloque de datos del archivo.
Puntero indirecto: Apunta a un bloque que contendrá los punteros a los siguientes bloques del archivo.
Nota: todos los punteros del FS se almacenarán como números enteros no signados de 4 bytes (uint32_t)


#endif /* FILESYSTEM_H_ */