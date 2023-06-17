 #include "segmentos.h"

Segmento* segmento_0;
int huecos_libres = -1;
char* string_algoritmo ;
char algoritmo;

int get_index_of_list(t_list* lista,int id){    
    for(int i = 0; i < list_size(lista);i++){
        Segmento* segmento = list_get(lista,i);
        if(segmento->id == id){
            return i;
        }
    }
    return -1;
}

Segmento* inicializar_segmento(int tamano){
    Segmento* segmento = malloc(sizeof(Segmento));
    segmento->inicio = malloc(tamano);
    segmento->tamano = tamano;
    return segmento;
}
void reservar_espacio_de_memoria(int tamano_memoria, int tamano_segmento_0){
    string_algoritmo = config_get_string_value(config,"ALGORITMO_ASIGNACION");
    algoritmo = string_algoritmo[0];
    espacio_de_memoria = malloc(tamano_memoria);
    lista_de_memoria = list_create();
    Segmento* hueco_libre = inicializar_segmento(tamano_memoria);
    hueco_libre->id = huecos_libres;
    hueco_libre->inicio = espacio_de_memoria; // aca habria que apuntar al primer byte, no a todo el espacio de memoria creo
    list_add(lista_de_memoria,hueco_libre);
    segmento_0 = crear_segmento(0,tamano_segmento_0);
}

int obtener_espacio_libre_total(){
    int espacio_libre;
    t_list* lista_de_huecos_libres = list_create();
    bool obtener_espacios_libres_de_la_memoria(void* elemento){
        Segmento* segmento = elemento;
        return segmento->id == -1;
    }
    void* calcular_espacio_libre(void* un_segmento, void* otro_segmento){
        Segmento* un_hueco_libre = un_segmento;
        Segmento* otro_hueco_libre = otro_segmento;
        espacio_libre = un_hueco_libre->tamano + otro_hueco_libre -> tamano;
        return &espacio_libre;
    }
    lista_de_huecos_libres = list_filter(lista_de_memoria,obtener_espacios_libres_de_la_memoria);
    int espacio_libre_total = *(int*)list_fold(lista_de_huecos_libres,list_get(lista_de_huecos_libres,0),calcular_espacio_libre);
    return espacio_libre_total;
}



Segmento* crear_segmento(int id, int tamano){
    bool hay_hueco_libre(void* un_segmento){
        Segmento* hueco_libre = un_segmento;
        return (hueco_libre->tamano >= tamano) && (hueco_libre->id < 0);
    }
    
    if(list_any_satisfy(lista_de_memoria,hay_hueco_libre)){
        Segmento* nuevo_segmento = inicializar_segmento(tamano);
        Segmento* hueco_libre = malloc(sizeof(Segmento));
        switch(algoritmo)
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
        nuevo_segmento->id = id;
        nuevo_segmento->inicio = hueco_libre->inicio;
        hueco_libre->inicio += tamano;
        hueco_libre->tamano -= tamano;
        if(hueco_libre->tamano <=0){
            free(hueco_libre->inicio);
            free(hueco_libre);
        }
        list_add_in_index(lista_de_memoria,get_index_of_list(lista_de_memoria,hueco_libre->id),nuevo_segmento);
        return nuevo_segmento;
       
    }
    if(obtener_espacio_libre_total() > tamano){ //Que hacia esto??
        Segmento* nuevo_segmento = inicializar_segmento(-1);
        return nuevo_segmento;
    }
    else{
        Segmento* nuevo_segmento = inicializar_segmento(-2);
        return nuevo_segmento;
    }
}

void eliminar_segmento(int id){
    bool obtener_segmento(void* elemento){
        Segmento* segmento = elemento;
        return segmento->id == id;
    }
    Segmento* segmento = list_find(lista_de_memoria,obtener_segmento);
    huecos_libres--;
    segmento->id = huecos_libres; 
    
    unificacion_de_huecos_libres();
}

void unificacion_de_huecos_libres(){
    for(int i = 0;  i+1 < list_size(lista_de_memoria); i++)
    {
        Segmento* seg1 = list_get(lista_de_memoria,i);
        Segmento* seg2 = list_get(lista_de_memoria,i+1);
        if ((seg1->id < 0) && (seg2->id < 0))
        {
            seg1->tamano += seg2->tamano;
            list_remove_and_destroy_element(lista_de_memoria, i+1, free);
        }
    }
}

Segmento* first_fit(int id, int tamano){
     bool hay_hueco_libre(void* un_segmento){
        Segmento* hueco_libre = un_segmento;
        return hueco_libre->tamano >= tamano && hueco_libre->id < 0;
    }
        Segmento* hueco_libre = malloc(sizeof(Segmento));
        hueco_libre = list_get(list_filter(lista_de_memoria,hay_hueco_libre),0);
        return hueco_libre;
}

Segmento* worst_fit(int id, int tamano){
     void* maximo_tamano(void* un_segmento, void* otro_segmento){
        Segmento* hueco_libre = un_segmento;
        Segmento* otro_hueco_libre = otro_segmento;
        if(hueco_libre->tamano > otro_hueco_libre->tamano)
        {
        return (void*)hueco_libre;
        }
        return (void*) otro_hueco_libre;
    }
        Segmento* hueco_libre = malloc(sizeof(Segmento));
        hueco_libre = list_get_maximum(lista_de_memoria,maximo_tamano);
        return hueco_libre;
}

Segmento* best_fit(int id, int tamano){
    void* minimo_tamano(void* un_segmento, void* otro_segmento){
        Segmento* hueco_libre = un_segmento;
        Segmento* otro_hueco_libre = otro_segmento;
        if(hueco_libre->tamano < otro_hueco_libre->tamano)
        {
        return (void*)hueco_libre;
        }
        return (void*) otro_hueco_libre;
    }
    bool huecos_con_tamano_necesario(void* un_segmento){
         Segmento* hueco_libre = un_segmento;
         return hueco_libre->tamano > tamano && hueco_libre-> id <0;
    }
        Segmento* hueco_libre = malloc(sizeof(Segmento));
        hueco_libre = list_get_minimum(list_filter(lista_de_memoria,huecos_con_tamano_necesario),minimo_tamano);
    return hueco_libre;
}


/*void* peticion_de_lectura(void* direccion_a_leer){
    char* leido = (char*)*direccion_a_leer; 
}*/



