#include "funciones_segmentos.h"

Segmento *segmento_0;
int huecos_libres = -1;
int cantidad_maxima_segmentos_por_proceso;
char *string_algoritmo;
char algoritmo;

Segmento *inicializar_segmento(int tamano)
{
    Segmento *segmento = malloc(sizeof(Segmento));
    segmento->tamano = tamano;
    return segmento;
}
void reservar_espacio_de_memoria(int tamano_memoria, int tamano_segmento_0)
{
    cantidad_maxima_segmentos_por_proceso = config_get_int_value(config, "CANT_SEGMENTOS");
    string_algoritmo = config_get_string_value(config, "ALGORITMO_ASIGNACION");
    algoritmo = string_algoritmo[0];
    espacio_de_memoria = malloc(tamano_memoria);
    lista_de_memoria = list_create();
    Segmento *hueco_libre = inicializar_segmento(tamano_memoria);
    hueco_libre->id = huecos_libres;
    hueco_libre->desplazamiento = 0;
    list_add(lista_de_memoria, hueco_libre);
    segmento_0 = crear_segmento(0, tamano_segmento_0, -1);
}

int obtener_espacio_libre_total()
{
    int espacio_libre;
    t_list *lista_de_huecos_libres = list_create();
    bool obtener_espacios_libres_de_la_memoria(void *elemento)
    {
        Segmento *segmento = elemento;
        return segmento->id < 0;
    }
    void *calcular_espacio_libre(void *un_segmento, void *otro_segmento)
    {
        Segmento *un_hueco_libre = un_segmento;
        Segmento *otro_hueco_libre = otro_segmento;
        espacio_libre = un_hueco_libre->tamano + otro_hueco_libre->tamano;
        return &espacio_libre;
    }
    lista_de_huecos_libres = list_filter(lista_de_memoria, obtener_espacios_libres_de_la_memoria);
    if (list_is_empty(lista_de_huecos_libres))
    {
        return 0;
    }
    int espacio_libre_total = *(int *)list_fold1(lista_de_huecos_libres, calcular_espacio_libre);
    return espacio_libre_total;
}

Segmento *crear_segmento(int id, int tamano, int pid)
{
    bool hay_hueco_libre(void *un_segmento)
    {
        Segmento *hueco_libre = un_segmento;
        return (hueco_libre->tamano >= tamano) && (hueco_libre->id < 0);
    }

    if (list_any_satisfy(lista_de_memoria, hay_hueco_libre))
    {
        Segmento *nuevo_segmento = inicializar_segmento(tamano);
        Segmento *hueco_libre = malloc(sizeof(Segmento));
        switch (algoritmo)
        {
        case 'F':
            hueco_libre = first_fit(id, tamano);
            break;
        case 'B':
            hueco_libre = best_fit(id, tamano);
            break;
        case 'W':
            hueco_libre = worst_fit(id, tamano);
            break;
        }
        nuevo_segmento->pid = pid;
        nuevo_segmento->id = id;
        nuevo_segmento->desplazamiento = hueco_libre->desplazamiento;
        hueco_libre->desplazamiento += tamano;
        hueco_libre->tamano -= tamano;

        list_add_in_index(lista_de_memoria, obtener_segmento_por_id(hueco_libre->id, lista_de_memoria), nuevo_segmento);
        if (hueco_libre->tamano <= 0)
        {
            list_remove_and_destroy_element(lista_de_memoria, obtener_segmento_por_id(hueco_libre->id, lista_de_memoria), free);
        }
        return nuevo_segmento;
    }
    if (obtener_espacio_libre_total() >= tamano)
    { // Me dice que tengo que compactar
        Segmento *nuevo_segmento = inicializar_segmento(-1);
        return nuevo_segmento;
    }
    else
    {
        Segmento *nuevo_segmento = inicializar_segmento(-2); // No tengo espacio
        return nuevo_segmento;
    }
}

void eliminar_segmento(t_contexto_de_ejecucion *contexto_de_ejecucion, int id)
{
    bool obtener_segmento(void *elemento)
    {
        Segmento *segmento = elemento;
        return segmento->id == id;
    }
    Segmento *segmento_a_eliminar = list_find(contexto_de_ejecucion->tabla_segmentos, obtener_segmento);
    if (segmento_a_eliminar == NULL)
    {
        return;
    }
    log_info(logger, "PID: %i - Eliminar Segmento: %i - Base: %i- TAMAÑO: %i", contexto_de_ejecucion->pid, segmento_a_eliminar->id, segmento_a_eliminar->desplazamiento, segmento_a_eliminar->tamano);

    int desplazamiento_segmento_a_eliminar = segmento_a_eliminar->desplazamiento;

    bool buscar_segmento_en_lista_de_memoria(void *elemento)
    {

        Segmento *segmento = elemento;
        return segmento->desplazamiento == desplazamiento_segmento_a_eliminar;
    }

    Segmento *segmento_en_lista_de_memoria = list_find(lista_de_memoria, buscar_segmento_en_lista_de_memoria);
    if (segmento_en_lista_de_memoria == NULL)
    {
        return;
    }
    segmento_en_lista_de_memoria->id = huecos_libres;

    unificacion_de_huecos_libres();
}

void unificacion_de_huecos_libres()
{
    int tamano_lista = list_size(lista_de_memoria);
    for (int i = 0; i + 1 < tamano_lista; i++)
    {
        Segmento *seg1 = list_get(lista_de_memoria, i);
        Segmento *seg2 = list_get(lista_de_memoria, i + 1);

        if (seg1->id < 0 && seg2->id < 0)
        {
            seg1->tamano += seg2->tamano;
            list_remove(lista_de_memoria, i + 1);
            i--;
            tamano_lista--;
        }
    }
}

Segmento *first_fit(int id, int tamano)
{
    bool hay_hueco_libre(void *un_segmento)
    {
        Segmento *hueco_libre = un_segmento;
        return hueco_libre->tamano >= tamano && hueco_libre->id < 0;
    }
    t_list *lista_filtrada = list_filter(lista_de_memoria, hay_hueco_libre);
    Segmento *hueco_libre = list_get(lista_filtrada, 0);
    return hueco_libre;
}

Segmento *worst_fit(int id, int tamano)
{
    void *maximo_tamano(void *un_segmento, void *otro_segmento)
    {
        Segmento *hueco_libre = un_segmento;
        Segmento *otro_hueco_libre = otro_segmento;
        return hueco_libre->tamano > otro_hueco_libre->tamano ? un_segmento : otro_segmento;
    }
    bool huecos_con_tamano_necesario(void *un_segmento)
    {
        Segmento *hueco_libre = un_segmento;
        return hueco_libre->tamano >= tamano && hueco_libre->id < 0;
    }
    t_list *lista_filtrada = list_filter(lista_de_memoria, huecos_con_tamano_necesario);

    Segmento *hueco_libre = (list_size(lista_filtrada) > 1) ? list_get_maximum(lista_filtrada, maximo_tamano) : list_get(lista_filtrada, 0);
    list_destroy(lista_filtrada);
    return hueco_libre;
}

Segmento *best_fit(int id, int tamano)
{
    void *minimo_tamano(void *un_segmento, void *otro_segmento)
    {
        Segmento *hueco_libre = un_segmento;
        Segmento *otro_hueco_libre = otro_segmento;
        return hueco_libre->tamano < otro_hueco_libre->tamano ? un_segmento : otro_segmento;
    }
    bool huecos_con_tamano_necesario(void *un_segmento)
    {
        Segmento *hueco_libre = un_segmento;
        return hueco_libre->tamano >= tamano && hueco_libre->id < 0;
    }
    t_list *lista_filtrada = list_filter(lista_de_memoria, huecos_con_tamano_necesario);

    Segmento *hueco_libre = (list_size(lista_filtrada) > 1) ? list_get_minimum(lista_filtrada, minimo_tamano) : list_get(lista_filtrada, 0);

    list_destroy(lista_filtrada);
    return hueco_libre;
}

t_list *compactar()
{
    t_list *segmentos_actualizados = list_create();
    bool ordenar_por_desplazamiento(void *segmento, void *segmento_dos)
    {
        Segmento *un_segmento = segmento;
        Segmento *otro_segmento = segmento_dos;
        return un_segmento->desplazamiento < otro_segmento->desplazamiento;
    }
    char *datos_a_copiar;
    int posicion_segmento_compactable;
    list_sort(lista_de_memoria, ordenar_por_desplazamiento);

    while ((posicion_segmento_compactable = buscar_segmento_compactable()) > 0)
    {
        Segmento *segmento_compactable = list_get(lista_de_memoria, posicion_segmento_compactable);
        datos_a_copiar = malloc(segmento_compactable->tamano);

        memcpy(datos_a_copiar, espacio_de_memoria + segmento_compactable->desplazamiento, segmento_compactable->tamano);
        Segmento *hueco_libre = list_get(lista_de_memoria, posicion_segmento_compactable - 1);
        memcpy(espacio_de_memoria + hueco_libre->desplazamiento, datos_a_copiar, segmento_compactable->tamano);
        free(datos_a_copiar);
        segmento_compactable->desplazamiento = hueco_libre->desplazamiento;
        hueco_libre->desplazamiento += segmento_compactable->tamano;
        list_add(segmentos_actualizados, segmento_compactable);
    }
    unificacion_de_huecos_libres();

    return segmentos_actualizados;
}

int buscar_segmento_compactable()
{
    int lista_size = list_size(lista_de_memoria);
    for (int i = 0; i < lista_size - 1; i++)
    {
        Segmento *un_segmento = list_get(lista_de_memoria, i);
        Segmento *otro_segmento = list_get(lista_de_memoria, i + 1);
        if (un_segmento->id < 0 && otro_segmento->id > 0)
        {
            return i + 1;
        }
    }
    return -1;
}
