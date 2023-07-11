#ifndef  SEGMENTOS_H_
#define  SEGMENTOS_H_

#include<commons/collections/list.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>

typedef struct {
    int tamano;
    int id;
    int desplazamiento;
}Segmento;

int obtener_segmento_por_id(int id, t_list* tabla_segmentos);
int traduccion_dir_logica_fisica(int dir_logica, t_list *tabla_segmentos, int tam_max_segmento);
char* int_a_string(int numero);

#endif  /* SEGMENTOS_H_ */