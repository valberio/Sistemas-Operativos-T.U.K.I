#ifndef BLOQUES_H_
#define BLOQUES_H_

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

extern t_log* logger;
extern t_bitarray *bitarray;
extern char *ruta_superbloque;
extern char *ruta_bitmap;
extern char *ruta_archivo_bloques;

typedef struct {
    int tamano;
    char* datos;
}t_bloque;

typedef struct {
    char* name;
    uint32_t size; //En bytes
    uint32_t direct_pointer; //Apunta al primer bloque de datos del archivo.
    uint32_t indirect_pointer; //Apunta a un bloque que contendrá los punteros a los siguientes bloques del archivo.
}t_fcb; 


void vaciar_archivo_bloques(double cant_bloques, double tamano_bloque);
void leer_archivo_de_bloques(double cant_bloques, double tamano_bloque);
void leer_bloque_completo(int nro_bloque, int tamano_bloque);
char* leer_bloque_desde_hasta(uint32_t nro_bloque, uint32_t desde, uint32_t hasta, int tamano_bloque);
char* completar_con_ceros(int index, int cant_bloques);
char* obtener_puntero_bloque_libre(int cant_bloques);
void escribir_en_archivo_de_bloques(char* ruta);
#endif /* BLOQUES_H_ */