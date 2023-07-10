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

extern t_log* logger;

typedef struct {
    double block_size;
    double block_count;
}t_superbloque;



//void *crear_fcb(char *nombre_archivo);
void crear_estructuras_fcb();
void recibir_ordenes_kernel(int conexion_filesystem_kernel);
void crear_estructura_fcb(char *nombre_archivo);
void* crear_archivo_fcb(char *nombre_archivo);
void abrir_o_crear_archivo(char *nombre_archivo, int conexion_filesystem_cliente);

int truncar_archivo(char *nombre_archivo, int nro_bloques, t_superbloque superbloque, t_bitarray *bitarray, int bloques, t_log *logger, FILE* archivo_de_bloques);
void guardar_fcb(FILE *archivo_fcb, t_fcb* fcb);
int liberar_bloques(FILE *archivo_de_bloques, t_fcb* fcb, t_bitarray* bitarray, int nro_bloques, t_superbloque superbloque);
void quitar_bloque_de_lista(FILE *archivo_de_bloques, t_bitarray *bitarray, t_superbloque superbloque, int indirect_pointer);
int buscar_bloque_disponible(t_bitarray *bitarray, t_superbloque superbloque);
void agregar_bloque_a_lista(FILE *archivo_de_bloques, t_bitarray *bitarray, t_superbloque superbloque, int indirect_pointer);
void escribir_bitmap(FILE *bitmap, t_bitarray *bitarray, t_superbloque t_superbloque);
void asignar_bloque(t_fcb *fcb, t_bitarray *bitarray, FILE *archivo_de_bloques, t_superbloque superbloque);
t_bitarray *leer_bitmap(FILE *bitmap, t_bitarray *bitarray, t_superbloque t_superbloque);
void escribir_bloque(FILE *archivo_de_bloques, t_bloque bloque, unsigned nro_bloque, t_superbloque superbloque);
t_bloque* leer_bloque(FILE *archivo_de_bloques, t_superbloque superbloque, unsigned nro_bloque);
t_list *leer_archivo(FILE *archivo_de_bloques, char *nombre_archivo, t_superbloque superbloque);
void escribir_archivo(FILE *archivo_de_bloques, char *nombre_archivo, uint32_t *data);

int obtener_digitos_cant_bloque();
#endif /* FILESYSTEM_H_ */