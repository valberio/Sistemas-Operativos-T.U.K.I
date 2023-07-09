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

 t_bitarray* crear_bitmap (char* ruta, double block_count);


#endif /* BITMAP_H_ */