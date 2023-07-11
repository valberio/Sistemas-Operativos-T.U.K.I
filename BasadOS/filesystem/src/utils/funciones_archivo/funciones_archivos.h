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
extern t_list *fcb_list;
extern t_bitarray *bitarray;
extern char *ruta_superbloque;
extern char *ruta_bitmap;
extern char *ruta_archivo_bloques;

void escribir_puntero_indirecto(t_fcb *, char *);
void truncar_archivo(char *, int);
void agrandar_archivo(t_fcb *, int);
int division_redondeada_hacia_arriba(int, int);
void remover_ultimo_bloque(t_fcb *);
void achicar_archivo(t_fcb *, int);

#endif /* FUNCIONES_ARCHIVOS_H_ */