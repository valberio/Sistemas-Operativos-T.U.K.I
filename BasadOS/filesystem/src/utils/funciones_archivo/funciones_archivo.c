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
    if(fcb_archivo->size == nuevo_tamanio)
    {
        log_info(logger, "No hace falta truncar %s porque ya tiene el tamaño necesario", fcb_archivo->name);
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
        log_info(logger, "No hace falta truncar el archivo %s porque su tamano %i es igual al nuevo tamano %i", fcb_archivo->name, fcb_archivo->size, nuevo_tamanio);
        return;
    }

    int bloques_por_agregar = division_redondeada_hacia_arriba(bytes_por_asignar, tamanio_bloque);
    log_info(logger, "Tengo que agregarle %i bloques al archivo %s", bloques_por_agregar, fcb_archivo->name);


    if (fcb_archivo->direct_pointer == -1)
    { // NO TENGA NADA Y ASIGNA PUNTERO DIRECTO
        bloques_por_agregar--;
        fcb_archivo->size += MIN(bytes_por_asignar, tamanio_bloque);

        bytes_por_asignar = MAX(bytes_por_asignar - tamanio_bloque, 0);

        uint32_t nuevo_bloque_directo = obtener_puntero_bloque_libre(cantidad_bloques);
        fcb_archivo->direct_pointer = nuevo_bloque_directo;

        log_info(logger, "Le agregue el puntero directo %i al archivo %s", nuevo_bloque_directo, fcb_archivo->name);

        setear_bit(fcb_archivo->direct_pointer);
    }
    if (fcb_archivo->direct_pointer >= 0 && fcb_archivo->indirect_pointer == -1 && bytes_por_asignar > 0)
    { // TENGA UN SOLO BLOQUE DIRECTO Y NECESITE MAS

        uint32_t nuevo_bloque_indirecto = obtener_puntero_bloque_libre(cantidad_bloques);

        fcb_archivo->indirect_pointer = nuevo_bloque_indirecto;
        log_info(logger, "Agregue al archivo %s el bloque %i, como bloque de indireccion", fcb_archivo->name, nuevo_bloque_indirecto);
        setear_bit(fcb_archivo->indirect_pointer);

        log_info(logger, "Acceso Bloque - Archivo: %s - Bloque Archivo: 1 - Bloque File System %i", fcb_archivo->name, fcb_archivo->indirect_pointer);
        sleep(retardo);
        while (bloques_por_agregar > 0) //Asigno bloques al bloque indirecto
        {
            uint32_t nuevo_bloque_datos = obtener_puntero_bloque_libre(cantidad_bloques);
            setear_bit(nuevo_bloque_datos);

            fcb_archivo->size += MIN(bytes_por_asignar, tamanio_bloque);

            escribir_puntero_indirecto(fcb_archivo, nuevo_bloque_datos);
            
            bytes_por_asignar = MAX(bytes_por_asignar - tamanio_bloque, 0);
            bloques_por_agregar--;
            log_info(logger, "Agregue al archivo %s el bloque %i", fcb_archivo->name, nuevo_bloque_datos);
            
        }
    }
    else if (bytes_por_asignar > 0)
    {//TIENE PUNTERO INDIRECTO YA EXISTENTE Y TIENE QUE AGREGAR MAS BLOQUES AHI
        log_info(logger, "Acceso Bloque - Archivo: %s - Bloque Archivo: 1 - Bloque File System %i", fcb_archivo->name, fcb_archivo->indirect_pointer);
        sleep(retardo);
        while (bloques_por_agregar > 0)
        {
            uint32_t nuevo_bloque_datos = obtener_puntero_bloque_libre(cantidad_bloques);

            setear_bit(nuevo_bloque_datos);

            fcb_archivo->size += MIN(bytes_por_asignar, tamanio_bloque);

            escribir_puntero_indirecto(fcb_archivo, nuevo_bloque_datos);

            
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

void escribir_puntero_indirecto(t_fcb *fcb, uint32_t puntero_a_escribir) //Actual problema: no se escriben bien las direcciones
{
    int cant_bloques_archivo = division_redondeada_hacia_arriba(fcb->size, tamanio_bloque);
    log_info(logger, "El archivo %s tiene %i bloques actualmente", fcb->name, cant_bloques_archivo);
    uint32_t puntero_indirecto = fcb->indirect_pointer;
    FILE *archivo_de_bloques = fopen(ruta_archivo_bloques, "r+");
    log_info(logger, "Al archivo le voy a guardar el bloque %i", puntero_a_escribir);
    fseek(archivo_de_bloques, (puntero_indirecto * tamanio_bloque) + (cant_bloques_archivo * sizeof(uint32_t)), SEEK_SET);
    fwrite(&puntero_a_escribir, sizeof(uint32_t), 1, archivo_de_bloques);

    uint32_t test;
    fseek(archivo_de_bloques, (puntero_indirecto * tamanio_bloque ) + (cant_bloques_archivo * sizeof(uint32_t)), SEEK_SET);
    fread(&test, sizeof(uint32_t), 1, archivo_de_bloques);
    log_info(logger, "Guarde en el puntero indirecto el bloque: %i", test);
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

void escribir_archivo(char* nombre, char *datos_a_guardar, int puntero, int cantidad_de_bytes) 
{
    bool es_el_fcb(void *elemento)
    {
        t_fcb *fcb_en_tabla;
        fcb_en_tabla = elemento;

        return strcmp(fcb_en_tabla->name, nombre) == 0;
    }

    t_fcb* fcb_archivo = list_find(fcb_list, es_el_fcb);
    if (fcb_archivo == NULL)
    {
        log_info(logger, "No se encontró el archivo");
        return;
    }
    
    FILE *archivo_de_bloques = fopen(ruta_archivo_bloques, "r+");
    
    int bytes_por_escribir = cantidad_de_bytes;
    int bytes_escritos
    
    if(puntero < tamanio_bloque)
    {
        //Escribo en el bloque directo

        int bytes_a_guardar = MIN(tamanio_bloque, cantidad_de_bytes);

        log_info(logger, "Acceso Bloque - Archivo: %s - Bloque Archivo: 0 - Bloque File System %i", fcb_archivo->name, fcb_archivo->direct_pointer);
        fseek(archivo_de_bloques, puntero * sizeof(char), SEEK_SET);
        fwrite(datos_a_guardar, sizeof(char), bytes_a_guardar, archivo_de_bloques);

        bytes_por_escribir -= bytes_a_guardar;

        puntero += bytes_a_guardar;
    }
    if(bytes_por_escribir > 0)
    {   
        log_info(logger, "Acceso Bloque - Archivo: %s - Bloque Archivo: 1 - Bloque File System %i", fcb_archivo->name, fcb_archivo->indirect_pointer);
        sleep(retardo);}
    
    while(bytes_por_escribir > 0)
    {
        
        int bloque_del_puntero = floor(puntero, tamanio_bloque);

        int bytes_a_escribir_aca = MIN(tamanio_bloque, bytes_por_escribir);
        uint32_t direccion_bloque;

        //BUSCO EL BLOQUE DEL PUNTERO INDIRECTO EN EL QUE HAY QUE ESCRIBIR
        fseek(archivo_de_bloques, fcb_archivo->indirect_pointer * sizeof(char) + bloque_del_puntero * sizeof(uint32_t), SEEK_SET);
        fwrite(&direccion_bloque, sizeof(uint32_t), 1, archivo_de_bloques);

        log_info(logger, "Voy a acceder al bloque %i, que saque del bloque de punteros indirectos", direccion_bloque);
        int byte_del_puntero_dentro_de_su_bloque = puntero - direccion_bloque * tamanio_bloque; 

        //Entro al bloque que me indica el indirecto, y escribo ahi
        fseek(archivo_de_bloques, (direccion_bloque * tamanio_bloque + byte_del_puntero_dentro_de_su_bloque )* sizeof(char), SEEK_SET);
        fwrite(datos_a_guardar + bytes_escritos, sizeof(char), bytes_a_escribir_aca, archivo_de_bloques);

        bytes_escritos += bytes_a_escribir_aca;
        bytes_por_escribir -= bytes_a_escribir_aca;

    }
}





/*
void escribir_archivo(char *nombre, char *datos_a_guardar, int puntero, int cantidad_de_bytes)
{
    bool es_el_fcb(void *elemento)
    {
        t_fcb *fcb_en_tabla;
        fcb_en_tabla = elemento;

        return strcmp(fcb_en_tabla->name, nombre) == 0;
    }

    t_fcb* fcb_archivo = list_find(fcb_list, es_el_fcb);
    if (fcb_archivo == NULL)
    {
        log_info(logger, "No se encontró el archivo");
        return;
    }
    int bloque_puntero = division_redondeada_hacia_arriba(puntero, tamanio_bloque);
    log_info(logger, "El puntero se encuentra en el bloque %i", bloque_puntero);
    int desplazamiento_dentro_del_bloque = puntero - (bloque_puntero * tamanio_bloque); //Posiblemente mal
    int bytes_por_copiar = cantidad_de_bytes;
    int bytes_a_copiar_aca;
    int bytes_copiados = 0;

    FILE *archivo_de_bloques = fopen(ruta_archivo_bloques, "r+");

    if (bloque_puntero == 0) //Esto significa que tengo que entrar al BLOQUE DIRECTO del FCB del archivo
    {
        int bytes_restantes_bloque_directo = tamanio_bloque - desplazamiento_dentro_del_bloque;
        bytes_a_copiar_aca = MIN(bytes_restantes_bloque_directo, cantidad_de_bytes);
        log_info(logger, "Acceso Bloque - Archivo: %s - Bloque Archivo: 0 - Bloque File System %i", fcb_archivo->name, fcb_archivo->direct_pointer);
        sleep(retardo);

        uint32_t bloque_directo_del_archivo = fcb_archivo->direct_pointer;

        fseek(archivo_de_bloques, (bloque_directo_del_archivo * tamanio_bloque) + desplazamiento_dentro_del_bloque + bytes_copiados, SEEK_SET);
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

        uint32_t index_bloque;

        // Entro al bloque indirecto, y leo el indice que corresponde
        fseek(archivo_de_bloques, (fcb_archivo->indirect_pointer * tamanio_bloque * sizeof(char)) + (contador_puntero_indirecto * sizeof(uint32_t)), SEEK_SET);
        fread(&index_bloque, sizeof(uint32_t), 1, archivo_de_bloques);
    
        log_info(logger, "Acceso Bloque - Archivo: %s - Bloque Archivo: %i - Bloque File System %i", fcb_archivo->name, contador_puntero_indirecto, index_bloque);
        sleep(retardo);
        
        fseek(archivo_de_bloques, (index_bloque * tamanio_bloque * sizeof(char)), SEEK_SET);
        fwrite(datos_a_guardar + bytes_copiados, sizeof(char), bytes_a_copiar_aca, archivo_de_bloques);

        bytes_copiados += bytes_a_copiar_aca;
        bytes_por_copiar -= bytes_a_copiar_aca;
        contador_puntero_indirecto++;

    }
    fclose(archivo_de_bloques);
}
*/
char *convertir_a_char(uint32_t numero)
{
    char *nuevo_valor = malloc(10 * sizeof(char));
    if (nuevo_valor != NULL)
    {
        sprintf(nuevo_valor, "%u", numero);
    }
    return nuevo_valor;
}