#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include<sockets/client_utils.h>
#include<sockets/server_utils.h>
#include<loggers/loggers_utils.h>
#include<pcb/pcb.h>
#include <commons/bitarray.h>
#include <math.h>
#include <libgen.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include "utils/bitmap/bitmap.h"
#include "utils/archivo_bloques/bloques.h"
#include "utils/funciones_archivo/funciones_archivos.h"


extern t_log* logger;



//void *crear_fcb(char *nombre_archivo);
void recorrer_directorio_fcb();
void recibir_ordenes_kernel(int conexion_filesystem_kernel);
void crear_estructura_fcb(char *nombre_archivo);
void* crear_archivo_fcb(char *nombre_archivo);
void abrir_o_crear_archivo(char *nombre_archivo, int conexion_filesystem_cliente);
int obtener_digitos_cant_bloque();


#endif /* FILESYSTEM_H_ */