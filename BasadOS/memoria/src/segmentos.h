#ifndef SEGMENTOS_H_
#define SEGMENTOS_H_

#include <commons/collections/list.h>
#include <commons/config.h>
#include<stdlib.h>
#include<stdio.h>

extern t_list* lista_de_memoria;
extern t_config* config;
extern void* espacio_de_memoria;

typedef struct {
    int tamano;
    int id;
    void* inicio;

} Segmento;

Segmento* inicializar_segmento(int );
int obtener_espacio_libre_total();
void reservar_espacio_de_memoria(int,int);
Segmento* crear_segmento(int ,int);
void eliminar_segmento(int );
int get_index_of_list(t_list* ,int );
void unificacion_de_huecos_libres();
Segmento* first_fit(int,int);
Segmento* worst_fit(int,int);
Segmento* best_fit(int,int);

#endif /* SEGMENTOS_H_ */