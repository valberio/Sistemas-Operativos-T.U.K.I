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
    segmento_0 = crear_segmento(0, tamano_segmento_0,-1);
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
    if(list_is_empty(lista_de_huecos_libres)){
        return 0;
    }
    int espacio_libre_total = *(int *)list_fold1(lista_de_huecos_libres, calcular_espacio_libre);
    log_info(logger, "el espacio libre total es: %d", espacio_libre_total);
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
    Segmento* segmento_a_eliminar = list_find(contexto_de_ejecucion->tabla_segmentos, obtener_segmento);
    if (segmento_a_eliminar == NULL) {
        log_info(logger, "El segmento no existe");
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
    if (segmento_en_lista_de_memoria == NULL) {
        log_info(logger, "El segmento no existe");
        return;
    }
    segmento_en_lista_de_memoria->id = huecos_libres;

    unificacion_de_huecos_libres();
}

void unificacion_de_huecos_libres()
{
    for (int i = 0; i + 1 < list_size(lista_de_memoria); i++)
    {
        Segmento *seg1 = list_get(lista_de_memoria, i);
        Segmento *seg2 = list_get(lista_de_memoria, i + 1);

        if ((seg1->id < 0) && (seg2->id < 0))
        {
            seg1->tamano += seg2->tamano;
            list_remove_and_destroy_element(lista_de_memoria, i + 1, free);
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
    Segmento *hueco_libre = malloc(sizeof(Segmento));
    hueco_libre = list_get(list_filter(lista_de_memoria, hay_hueco_libre), 0);
    return hueco_libre;
}

Segmento *worst_fit(int id, int tamano)
{
    void *maximo_tamano(void *un_segmento, void *otro_segmento)
    {
        Segmento *hueco_libre = un_segmento;
        Segmento *otro_hueco_libre = otro_segmento;
        if (hueco_libre->tamano > otro_hueco_libre->tamano && hueco_libre->id < 0 && otro_hueco_libre->id < 0)
        {
            return (void *)hueco_libre;
        }
        return (void *)otro_hueco_libre;
    }
    Segmento *hueco_libre = malloc(sizeof(Segmento));
    hueco_libre = list_get_maximum(lista_de_memoria, maximo_tamano);
    return hueco_libre;
}

Segmento *best_fit(int id, int tamano)
{
    void *minimo_tamano(void *un_segmento, void *otro_segmento)
    {
        Segmento *hueco_libre = un_segmento;
        Segmento *otro_hueco_libre = otro_segmento;
        if (hueco_libre->tamano < otro_hueco_libre->tamano)
        {
            return (void *)hueco_libre;
        }
        return (void *)otro_hueco_libre;
    }
    bool huecos_con_tamano_necesario(void *un_segmento)
    {
        Segmento *hueco_libre = un_segmento;
        return hueco_libre->tamano > tamano && hueco_libre->id < 0;
    }
    Segmento *hueco_libre = malloc(sizeof(Segmento));
    t_list *lista_filtrada = list_filter(lista_de_memoria, huecos_con_tamano_necesario);
    hueco_libre = list_get_minimum(lista_filtrada, minimo_tamano);
    list_destroy(lista_filtrada);
    return hueco_libre;
}

void inicializar_proceso(t_contexto_de_ejecucion *contexto_de_ejecucion, int conexion_memoria_kernel)
{
    list_add(contexto_de_ejecucion->tabla_segmentos, segmento_0);
    enviar_contexto_de_ejecucion(contexto_de_ejecucion, conexion_memoria_kernel);
}

t_list *compactar()
{
    t_list *segmentos_actualizados = list_create();
    bool ordenar_por_desplazamiento(void *segmento, void *segmento_dos)
    {
        Segmento *un_segmento = (Segmento *)segmento;
        Segmento *otro_segmento = (Segmento *)segmento_dos;
        return un_segmento->desplazamiento < otro_segmento->desplazamiento;
    }

    int posicion_segmento_compactable;
    while ((posicion_segmento_compactable = buscar_segmento_compactable()) > 0)
    {
        Segmento *segmento_compactable = list_get(lista_de_memoria, posicion_segmento_compactable);
        Segmento *hueco_libre = list_get(lista_de_memoria, posicion_segmento_compactable - 1);
        segmento_compactable->desplazamiento = hueco_libre->desplazamiento;
        hueco_libre->desplazamiento += segmento_compactable->tamano;
        list_add(segmentos_actualizados, segmento_compactable);
        list_sort(lista_de_memoria, ordenar_por_desplazamiento);
        unificacion_de_huecos_libres();
    }

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
