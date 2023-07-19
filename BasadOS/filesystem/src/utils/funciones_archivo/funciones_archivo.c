#include "funciones_archivos.h"
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

void truncar_archivo(char *nombre_archivo, int nuevo_tamanio)
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
        achicar_archivo(fcb_archivo, nuevo_tamanio);
    }
    else if (fcb_archivo->size < nuevo_tamanio)
    {
        agrandar_archivo(fcb_archivo, nuevo_tamanio);
    }
    t_config *config = iniciar_config(fcb_archivo->ruta);
    char *size_str = convertir_a_char(fcb_archivo->size);
    char *direct_pointer_str = convertir_a_char(fcb_archivo->direct_pointer);

    char *indirect_pointer_str = "-1";
    if (fcb_archivo->indirect_pointer != -1)
    {
        indirect_pointer_str = convertir_a_char(fcb_archivo->indirect_pointer);
    };

    config_set_value(config, "TAMANIO_ARCHIVO", size_str);
    config_set_value(config, "PUNTERO_DIRECTO", direct_pointer_str);
    config_set_value(config, "PUNTERO_INDIRECTO", indirect_pointer_str);
    config_save(config);

    config_destroy(config);
    free(size_str);
    free(direct_pointer_str);
    if (fcb_archivo->indirect_pointer != -1)
    {
        free(indirect_pointer_str);
    }
}

int division_redondeada_hacia_arriba(int dividendo, int divisor)
{
    int resultado = dividendo / divisor;
    int residuo = dividendo % divisor;

    if (residuo > 0)
    {
        resultado++;
    }

    return resultado;
}

void agrandar_archivo(t_fcb *fcb_archivo, int nuevo_tamanio)
{
    uint32_t bytes_por_asignar = nuevo_tamanio - fcb_archivo->size;

    // Agrego un caso mas: tengo un bloque incompleto en el que puedo asignar bytes
    int cantidad_bytes_teoricos_archivo = division_redondeada_hacia_arriba(fcb_archivo->size, tamanio_bloque) * tamanio_bloque;
    int bytes_libres_en_bloque = cantidad_bytes_teoricos_archivo - fcb_archivo->size;
    if ((bytes_libres_en_bloque) > 0)
    {
        fcb_archivo->size += bytes_libres_en_bloque;
        bytes_por_asignar = bytes_por_asignar - bytes_libres_en_bloque;
    }

    if (bytes_por_asignar == 0)
    {
        return;
    }

    int bloques_por_agregar = division_redondeada_hacia_arriba(bytes_por_asignar, tamanio_bloque);

    if (fcb_archivo->direct_pointer == -1)
    { // NO TENGA NADA Y ASIGNA PUNTERO DIRECTO
        bloques_por_agregar--;
        fcb_archivo->size += MIN(bytes_por_asignar, tamanio_bloque);

        bytes_por_asignar = MAX(bytes_por_asignar - tamanio_bloque, 0);

        int nuevo_bloque_directo = obtener_puntero_bloque_libre(cantidad_bloques);
        fcb_archivo->direct_pointer = nuevo_bloque_directo;
        setear_bit(fcb_archivo->direct_pointer);
    }
    if (fcb_archivo->direct_pointer >= 0 && fcb_archivo->indirect_pointer == -1 && bytes_por_asignar > 0)
    { // TENGA UN SOLO BLOQUE DIRECTO Y NECESITE MAS

        int nuevo_bloque_indirecto = obtener_puntero_bloque_libre(cantidad_bloques);

        fcb_archivo->indirect_pointer = nuevo_bloque_indirecto;
        setear_bit(fcb_archivo->indirect_pointer);

        log_info(logger, "Acceso Bloque - Archivo: %s - Bloque Archivo: 1 - Bloque File System %i", fcb_archivo->name, fcb_archivo->indirect_pointer);
        sleep(retardo);
        while (bloques_por_agregar > 0)
        {
            int nuevo_bloque_datos = obtener_puntero_bloque_libre();
            setear_bit(nuevo_bloque_datos);
            escribir_puntero_indirecto(fcb_archivo, nuevo_bloque_datos);
            fcb_archivo->size += MIN(bytes_por_asignar, tamanio_bloque);
            bytes_por_asignar = MAX(bytes_por_asignar - tamanio_bloque, 0);
            bloques_por_agregar--;
        }
    }
    else if (bytes_por_asignar > 0)
    {//TIENE PUNTERO INDIRECTO YA EXISTENTE Y TIENE QUE AGREGAR MAS BLOQUES AHI
        log_info(logger, "Acceso Bloque - Archivo: %s - Bloque Archivo: 1 - Bloque File System %i", fcb_archivo->name, fcb_archivo->indirect_pointer);
        sleep(retardo);
        while (bloques_por_agregar > 0)
        {
            int nuevo_bloque_datos = obtener_puntero_bloque_libre();

            setear_bit(nuevo_bloque_datos);
            escribir_puntero_indirecto(fcb_archivo, nuevo_bloque_datos);

            fcb_archivo->size += MIN(bytes_por_asignar, tamanio_bloque);
            bytes_por_asignar = MAX(bytes_por_asignar - tamanio_bloque, 0);
            bloques_por_agregar--;

        }
    }
}

void remover_ultimo_bloque(t_fcb *fcb_archivo)
{
    int cant_bloques_asignados = division_redondeada_hacia_arriba(fcb_archivo->size, tamanio_bloque);
    //int digitos_punteros = obtener_digitos_cant_bloque(cantidad_bloques);

    if (fcb_archivo->indirect_pointer == -1)
    {
        limpiar_bit(fcb_archivo->direct_pointer);

        fcb_archivo->direct_pointer = -1;
    }
    else
    {
        uint32_t bloque_indirecto = fcb_archivo->indirect_pointer;
        FILE *archivo_de_bloques = fopen(ruta_archivo_bloques, "r+");
        log_info(logger, "Acceso Bloque - Archivo: %s - Bloque Archivo: 1 - Bloque File System %i", fcb_archivo->name, fcb_archivo->indirect_pointer);
        sleep(retardo);
        fseek(archivo_de_bloques, (bloque_indirecto * tamanio_bloque) + ((cant_bloques_asignados - 1) * sizeof(int)), SEEK_SET);
        // log_info(logger, "Digitos punteros %i", digitos_punteros);

        int ultimo_puntero;
        fread(&ultimo_puntero, sizeof(int), 1, archivo_de_bloques);

        limpiar_bit(ultimo_puntero);


        int puntero_vacio = 0;

        fseek(archivo_de_bloques, (bloque_indirecto * tamanio_bloque) + ((cant_bloques_asignados - 1) * sizeof(int)), SEEK_SET);
        fwrite(&puntero_vacio, sizeof(int), 1, archivo_de_bloques);

        fclose(archivo_de_bloques);

    }
}

void achicar_archivo(t_fcb *fcb_archivo, int nuevo_tamanio)
{
    int bytes_por_sacar = fcb_archivo->size - nuevo_tamanio;

    int cantidad_bloques_asignados = division_redondeada_hacia_arriba(fcb_archivo->size, tamanio_bloque);
    int cant_bytes_teorica = cantidad_bloques_asignados * tamanio_bloque;
    int bytes_bloque_incompleto = tamanio_bloque - (cant_bytes_teorica - fcb_archivo->size);

    // Me fijo si la cantidad de bytes que tengo que remover es menor a un bloque
    // Tengo que modificar el tamaño en el fcb unicamente
    if (bytes_bloque_incompleto > 0)
    {
        fcb_archivo->size -= bytes_bloque_incompleto;
        bytes_por_sacar -= bytes_bloque_incompleto;

        // Me fijo si tengo que sacar un bloque -> si el tamaño del archivo quedo como un multiplo del tamano del
        // bloque, tengo que sacarle el ultimo bloque
        if ((fcb_archivo->size % tamanio_bloque) == 0)
        {
            remover_ultimo_bloque(fcb_archivo);
        }
    }
    while (bytes_por_sacar >= tamanio_bloque)
    {
        fcb_archivo->size -= tamanio_bloque;
        bytes_por_sacar -= tamanio_bloque;
        remover_ultimo_bloque(fcb_archivo);
    }
    if (bytes_por_sacar < tamanio_bloque && bytes_por_sacar > 0)
    {
        fcb_archivo->size -= bytes_por_sacar;
    }
}

void escribir_puntero_indirecto(t_fcb *fcb, int puntero_a_escribir)
{
    int cant_bloques_archivo = division_redondeada_hacia_arriba(fcb->size, tamanio_bloque) - 1;

    uint32_t puntero_indirecto = fcb->indirect_pointer;
    FILE *archivo_de_bloques = fopen(ruta_archivo_bloques, "r+");

    fseek(archivo_de_bloques, (puntero_indirecto * tamanio_bloque) + (cant_bloques_archivo * sizeof(int)), SEEK_SET);
    fwrite(&puntero_a_escribir, sizeof(int), 1, archivo_de_bloques);
    fclose(archivo_de_bloques);
}

char *leer_archivo(char *nombre, int puntero, int cantidad_de_bytes)
{
    bool es_el_fcb(void *elemento)
    {
        t_fcb *fcb_en_tabla;
        fcb_en_tabla = elemento;

        return strcmp(fcb_en_tabla->name, nombre) == 0;
    }

    t_fcb *fcb_archivo = list_find(fcb_list, es_el_fcb);
    if (fcb_archivo == NULL)
    {
        log_info(logger, "No se encontró el archivo");
        return "ERROR";
    }
    // Me fijo en que bloque esta el puntero
    int bloque_puntero = floor(puntero / tamanio_bloque);
    int desplazamiento_dentro_del_bloque = puntero - (bloque_puntero * tamanio_bloque);
    int bytes_por_leer = cantidad_de_bytes;
    char *datos = malloc((cantidad_de_bytes + 1) * sizeof(char));
    int bytes_a_leer_aca;
    int bytes_leidos = 0;
    FILE *archivo_de_bloques = fopen(ruta_archivo_bloques, "r");

    if (bloque_puntero == 0) // Accedo y leo del bloque directo
    {

        int bytes_restantes_bloque_directo = tamanio_bloque - desplazamiento_dentro_del_bloque;
        bytes_a_leer_aca = MIN(bytes_restantes_bloque_directo, cantidad_de_bytes);
        log_info(logger, "Acceso Bloque - Archivo: %s - Bloque Archivo: 0 - Bloque File System %i", fcb_archivo->name, fcb_archivo->direct_pointer);
        sleep(retardo);

        fseek(archivo_de_bloques, (bloque_puntero * tamanio_bloque) + desplazamiento_dentro_del_bloque, SEEK_SET);
        fread(datos + bytes_leidos, sizeof(char), bytes_a_leer_aca, archivo_de_bloques);

        bytes_por_leer -= bytes_a_leer_aca;
        bytes_leidos += bytes_a_leer_aca;
    }

    int contador_puntero_indirecto = bloque_puntero - 1;
    
    if (bytes_por_leer > 0)
    {
        log_info(logger, "Acceso Bloque - Archivo: %s - Bloque Archivo: 1 - Bloque File System %i", fcb_archivo->name, fcb_archivo->indirect_pointer);
        sleep(retardo);
    }
    int bloque_indirecto_contador = 2; // Solo para el log, no tiene funcionalidad.
    while (bytes_por_leer > 0)         // Leo bloques del indirecto
    {
        bytes_a_leer_aca = MIN(tamanio_bloque, cantidad_de_bytes);

        int index_bloque;

        // Entro al bloque indirecto, y leo el indice que corresponde
        fseek(archivo_de_bloques, fcb_archivo->indirect_pointer * tamanio_bloque + contador_puntero_indirecto * sizeof(int), SEEK_SET);
        fread(&index_bloque, sizeof(int), 1, archivo_de_bloques);


        log_info(logger, "Acceso Bloque - Archivo: %s - Bloque Archivo: %i - Bloque File System %i", fcb_archivo->name, bloque_indirecto_contador, index_bloque);
        sleep(retardo);

        fseek(archivo_de_bloques, (index_bloque * tamanio_bloque), SEEK_SET);
        fread(datos + bytes_leidos, sizeof(char), bytes_a_leer_aca, archivo_de_bloques);

        bytes_por_leer -= bytes_a_leer_aca;
        contador_puntero_indirecto++;
        bloque_indirecto_contador++;
        bytes_leidos += bytes_a_leer_aca;
    }
    fclose(archivo_de_bloques);
    datos[cantidad_de_bytes] = '\0';
    return datos;
}

void escribir_archivo(char *nombre, char *datos_a_guardar, int puntero, int cantidad_de_bytes)
{
    bool es_el_fcb(void *elemento)
    {
        t_fcb *fcb_en_tabla;
        fcb_en_tabla = elemento;

        return strcmp(fcb_en_tabla->name, nombre) == 0;
    }

    t_fcb *fcb_archivo = list_find(fcb_list, es_el_fcb);
    if (fcb_archivo == NULL)
    {
        log_info(logger, "No se encontró el archivo");
        return;
    }
    int bloque_puntero = floor(puntero / tamanio_bloque);
    int desplazamiento_dentro_del_bloque = puntero - (bloque_puntero * tamanio_bloque);
    int bytes_por_copiar = cantidad_de_bytes;
    int bytes_a_copiar_aca;
    int bytes_copiados = 0;

    FILE *archivo_de_bloques = fopen(ruta_archivo_bloques, "r+");

    if (bloque_puntero == 0)
    {
        int bytes_restantes_bloque_directo = tamanio_bloque - desplazamiento_dentro_del_bloque;
        bytes_a_copiar_aca = MIN(bytes_restantes_bloque_directo, cantidad_de_bytes);
        log_info(logger, "Acceso Bloque - Archivo: %s - Bloque Archivo: 0 - Bloque File System %i", fcb_archivo->name, fcb_archivo->direct_pointer);
        sleep(retardo);
        fseek(archivo_de_bloques, (bloque_puntero * tamanio_bloque) + desplazamiento_dentro_del_bloque + bytes_copiados, SEEK_SET);
        fwrite(datos_a_guardar + bytes_copiados, sizeof(char), bytes_a_copiar_aca, archivo_de_bloques);
        bytes_copiados += bytes_a_copiar_aca;
        bytes_por_copiar -= bytes_a_copiar_aca;
    }
    int contador_puntero_indirecto = bloque_puntero - 1;
 
    if (bytes_por_copiar > 0 && bytes_copiados < cantidad_de_bytes)
        {
            log_info(logger, "Acceso Bloque - Archivo: %s - Bloque Archivo: 1 - Bloque File System %i", fcb_archivo->name, fcb_archivo->indirect_pointer);
            sleep(retardo);
        }
        
        
    while (bytes_por_copiar > 0 && bytes_copiados < cantidad_de_bytes)
    {
        bytes_a_copiar_aca = MIN(tamanio_bloque, cantidad_de_bytes);

        int index_bloque;

        // Entro al bloque indirecto, y leo el indice que corresponde
        fseek(archivo_de_bloques, fcb_archivo->indirect_pointer * tamanio_bloque + contador_puntero_indirecto * sizeof(int), SEEK_SET);
        fread(&index_bloque, sizeof(int), 1, archivo_de_bloques);
    
        log_info(logger, "Acceso Bloque - Archivo: %s - Bloque Archivo: %i - Bloque File System %i", fcb_archivo->name, contador_puntero_indirecto + 2, index_bloque);
        sleep(retardo);
        
        fseek(archivo_de_bloques, (index_bloque * tamanio_bloque), SEEK_SET);
        fwrite(datos_a_guardar + bytes_copiados, sizeof(char), bytes_a_copiar_aca, archivo_de_bloques);

        bytes_copiados += bytes_a_copiar_aca;
        bytes_por_copiar -= bytes_a_copiar_aca;
        contador_puntero_indirecto++;

    }
    fclose(archivo_de_bloques);
}

char *convertir_a_char(uint32_t numero)
{
    char *nuevo_valor = malloc(10 * sizeof(char));
    if (nuevo_valor != NULL)
    {
        sprintf(nuevo_valor, "%u", numero);
    }
    return nuevo_valor;
}