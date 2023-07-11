#include "funciones_archivos.h"
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))


void truncar_archivo(char *nombre_archivo, int nuevo_tamanio, char *ruta_bitmap, char* ruta_archivo_bloques)
{
    bool es_el_fcb(void *elemento)
    {
        t_fcb *fcb_en_tabla;
        fcb_en_tabla = elemento;

        return strcmp(fcb_en_tabla->name, nombre_archivo) == 0;
    }
    t_fcb *fcb_archivo = list_find(fcb_list, es_el_fcb);
    // REVISAR SI ESTA NULO O SI YA TIENE POINTERS
    if (fcb_archivo->size > nuevo_tamanio)
    {
       // achicar_archivo(fcb_archivo, nuevo_tamanio);
    }
    else if (fcb_archivo->size < nuevo_tamanio)
    {
        agrandar_archivo(fcb_archivo, nuevo_tamanio,ruta_bitmap, ruta_archivo_bloques);
    }
}


int division_redondeada_hacia_arriba(int dividendo, int divisor) {
    int resultado = dividendo / divisor;
    int residuo = dividendo % divisor;

    if (residuo > 0) {
        resultado++;
    }

    return resultado;
}
void agrandar_archivo(t_fcb *fcb_archivo, int nuevo_tamanio, char *ruta_bitmap, char* ruta_archivo_bloques)
{
    log_info(logger, "Tamanio inicial %i", fcb_archivo->size);
    uint32_t bytes_por_asignar = nuevo_tamanio - fcb_archivo->size;

    //Agrego un caso mas: tengo un bloque incompleto en el que puedo asignar bytes
    int cantidad_bytes_teoricos_archivo = division_redondeada_hacia_arriba(fcb_archivo->size, tamanio_bloque) * tamanio_bloque;
    int bytes_libres_en_bloque = cantidad_bytes_teoricos_archivo - fcb_archivo->size;
    log_info(logger, "Hay %i bytes libres en el bloque", bytes_libres_en_bloque);
    if((bytes_libres_en_bloque) > 0)
    {
        fcb_archivo->size += bytes_libres_en_bloque;
        bytes_por_asignar = bytes_por_asignar - bytes_libres_en_bloque;
    }

    if (bytes_por_asignar == 0)
    {
        log_info(logger, "Hay %i bytes por asignar y el tamaño final del fcb es %i", bytes_por_asignar, fcb_archivo->size);
        return NULL;
    }

    int bloques_por_agregar = division_redondeada_hacia_arriba(bytes_por_asignar, tamanio_bloque); 

    log_info(logger, "Bytes por asignar %i Bloques por agregar %i", bytes_por_asignar, bloques_por_agregar);

    if (fcb_archivo->direct_pointer < 0)
    { // NO TENGA NADA
        bloques_por_agregar--;
        bytes_por_asignar = MAX(bytes_por_asignar - tamanio_bloque, 0);
        char *nuevo_bloque_directo = obtener_puntero_bloque_libre(cantidad_bloques, bitarray);
        fcb_archivo->direct_pointer = atoi(nuevo_bloque_directo);
        setear_bit(fcb_archivo->direct_pointer, bitarray, ruta_bitmap);
        fcb_archivo->size += MIN(bytes_por_asignar, tamanio_bloque);
        log_info(logger, "Tamaño del fcb %i bytes por asignar %i bloques por asignar %i", fcb_archivo->size, bytes_por_asignar, bloques_por_agregar);
    }
    else if (fcb_archivo->direct_pointer > 0  && fcb_archivo->indirect_pointer < 0)
    { // TENGA UN SOLO BLOQUE
        char *nuevo_bloque_indirecto = obtener_puntero_bloque_libre(cantidad_bloques, bitarray);
        fcb_archivo->indirect_pointer = atoi(nuevo_bloque_indirecto);
        setear_bit(fcb_archivo->indirect_pointer, bitarray, ruta_bitmap);
        while (bloques_por_agregar > 0)
        {
            char *nuevo_bloque_datos = obtener_puntero_bloque_libre(cantidad_bloques, bitarray);
            setear_bit(atoi(nuevo_bloque_datos), bitarray, ruta_bitmap);
            escribir_puntero_indirecto(fcb_archivo, nuevo_bloque_datos,ruta_archivo_bloques);
            fcb_archivo->size += MIN(bytes_por_asignar, tamanio_bloque);
            bytes_por_asignar = MAX(bytes_por_asignar - tamanio_bloque, 0);
            bloques_por_agregar--;
            log_info(logger, "Tamaño del fcb %i bytes por asignar %i bloques por asignar %i", fcb_archivo->size, bytes_por_asignar, bloques_por_agregar);
        }
    }
    else{
        while (bloques_por_agregar > 0) {



            char *nuevo_bloque_datos = obtener_puntero_bloque_libre(cantidad_bloques, bitarray);
            

            setear_bit(atoi(nuevo_bloque_datos), bitarray, ruta_bitmap);
            escribir_puntero_indirecto(fcb_archivo, nuevo_bloque_datos, ruta_archivo_bloques);
            fcb_archivo->size += MIN(bytes_por_asignar, tamanio_bloque);
            bytes_por_asignar = MAX(bytes_por_asignar - tamanio_bloque, 0);
            bloques_por_agregar--;
            log_info(logger, "Tamaño del fcb %i bytes por asignar %i bloques por asignar %i", fcb_archivo->size, bytes_por_asignar, bloques_por_agregar);
        }
    }
}

// void achicar_archivo(t_fcb *fcb_archivo, int nuevo_tamanio, char* ruta)
// {
// }

void escribir_puntero_indirecto(t_fcb *fcb, char *puntero_a_escribir, char* ruta_archivo_bloques)
{
    int cant_bloques_archivo = ceil(fcb->size / tamanio_bloque) - 1;

    uint32_t puntero_indirecto = fcb->indirect_pointer;
    int digitos_punteros = obtener_digitos_cant_bloque();
    FILE *archivo_de_bloques = fopen(ruta_archivo_bloques, "w+");

    fseek(archivo_de_bloques, (puntero_indirecto * tamanio_bloque) + (cant_bloques_archivo * digitos_punteros), SEEK_SET);
    fwrite(puntero_a_escribir, digitos_punteros * sizeof(char), 1, archivo_de_bloques);
    fclose(archivo_de_bloques);
}