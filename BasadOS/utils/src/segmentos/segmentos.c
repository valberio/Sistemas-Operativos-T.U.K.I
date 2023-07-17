#include "segmentos.h"

int obtener_segmento_por_id(int id, t_list *tabla_segmentos)
{
    int tam = list_size(tabla_segmentos);
    int index = -1;
    for (int i = 0; i < tam; i++)
    {
        Segmento *temp = list_get(tabla_segmentos, i);
        if (temp->id == id)
        {
            index = i;
        }
    }
    return index;
}

int traduccion_dir_logica_fisica(int dir_logica, t_list *tabla_segmentos, int tam_max_segmento, int bytes_a_escribir)
{
    int num_segmento = floor(dir_logica / tam_max_segmento);
    int desplazamiento_segmento = dir_logica % tam_max_segmento;

    if (list_size(tabla_segmentos) != 0)
    {
        int segmento_index = obtener_segmento_por_id(num_segmento, tabla_segmentos);
        Segmento *segmento = list_get(tabla_segmentos, segmento_index);
        int dir_fisica = segmento->desplazamiento + desplazamiento_segmento;

        if ((desplazamiento_segmento + bytes_a_escribir) > segmento->tamano) // Caso de SEGFAULT
        {
            dir_fisica = -1;
        }
        return dir_fisica;
    }
    return 0;
}

char* int_a_string(int numero) {
    int longitud = snprintf(NULL, 0, "%d", numero); 
    char* str = malloc((longitud + 1) * sizeof(char)); 
    sprintf(str, "%d", numero); 
    return str;
}