#ifndef BITMAP_H_
#define BITMAP_H_

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
#include <unistd.h>

extern t_bitarray *bitarray;
extern char *ruta_bitmap;
extern double retardo;

t_bitarray* crear_bitmap (char* ruta, double block_count);
char ver_bit(int index);
void limpiar_bit(int index);
void setear_bit(int index);


#endif /* BITMAP_H_ */