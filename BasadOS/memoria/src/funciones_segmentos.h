#ifndef FUNCIONES_SEGMENTOS_H_
#define FUNCIONES_SEGMENTOS_H_

#include <commons/collections/list.h>
#include <commons/config.h>
#include<stdlib.h>
#include<stdio.h>
#include<pcb/pcb.h>

extern t_list* lista_de_memoria;
extern t_config* config;
extern t_log* logger;
extern void* espacio_de_memoria;

Segmento* inicializar_segmento(int );
int obtener_espacio_libre_total();
void reservar_espacio_de_memoria(int,int);
Segmento* crear_segmento(int id ,int tamano);
void eliminar_segmento(t_contexto_de_ejecucion* contexto_de_ejecucions, int id);
int get_index_of_list(t_list* ,int );
void unificacion_de_huecos_libres();
Segmento* first_fit(int,int);
Segmento* worst_fit(int,int);
Segmento* best_fit(int,int);
int buscar_segmento_compactable();
t_list* compactar();



#endif /* FUNCIONES_SEGMENTOS_H_ */
