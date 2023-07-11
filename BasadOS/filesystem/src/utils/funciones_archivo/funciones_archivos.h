#ifndef FUNCIONES_ARCHIVOS_H_
#define FUNCIONES_ARCHIVOS_H_



#include <sockets/client_utils.h>
#include <sockets/server_utils.h>
#include <loggers/loggers_utils.h>
#include <pcb/pcb.h>
#include <commons/bitarray.h>
#include <math.h>
#include <libgen.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include "../../main_filesystem.h"

extern uint32_t tamanio_bloque;
extern uint32_t cantidad_bloques;
extern t_list* fcb_list;
extern t_bitarray* bitarray;

void escribir_puntero_indirecto(t_fcb *fcb, char *puntero_a_escribir, char* ruta_archivo_bloques);
void truncar_archivo(char *nombre_archivo, int nuevo_tamanio, char *ruta_bitmap, char* ruta_archivo_bloques);
void agrandar_archivo(t_fcb *fcb_archivo, int nuevo_tamanio, char *ruta_bitmap, char* ruta_archivo_bloques);
int division_redondeada_hacia_arriba(int dividendo, int divisor);


#endif /* FUNCIONES_ARCHIVOS_H_ */