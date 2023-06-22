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

typedef struct {
    double block_size;
    double block_count;
}t_superbloque;

typedef struct {
    uint32_t* data;
}t_bloque;

typedef struct {
    char* name;
    uint32_t size; //En bytes
    uint32_t direct_pointer; //Apunta al primer bloque de datos del archivo.
    uint32_t indirect_pointer; //Apunta a un bloque que contendrá los punteros a los siguientes bloques del archivo.
}t_fcb; 

//void *crear_fcb(char *nombre_archivo);
void crear_estructuras_fcb(); 
void leer_fcb(char *nombre_archivo);
int crear_archivo(char *nombre_archivo, t_superbloque *superbloque, t_bitarray *bitarray);
int abrir_archivo(char *nombre_archivo, t_superbloque superbloque, t_bitarray *bitarray);
int truncar_archivo(char *nombre_archivo, int nro_bloques, t_superbloque superbloque, t_bitarray *bitarray, int bloques, t_log *logger, FILE* archivo_de_bloques);
void guardar_fcb(FILE *archivo_fcb, t_fcb fcb);
int liberar_bloques(FILE *archivo_de_bloques, t_fcb fcb, t_bitarray* bitarray, int nro_bloques, t_superbloque superbloque);
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
#endif /* FILESYSTEM_H_ */