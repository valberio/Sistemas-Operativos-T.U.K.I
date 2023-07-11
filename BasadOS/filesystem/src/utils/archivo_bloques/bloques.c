#include "bloques.h"

void vaciar_archivo_bloques(double cant_bloques, double tamano_bloque, char* ruta_archivo_bloques)
{
    FILE* archivo_bloques = fopen(ruta_archivo_bloques, "w");
    fseek(archivo_bloques, 0, SEEK_SET);
    if(archivo_bloques == NULL)
    {
        log_info(logger, "Error abriendo el archivo %s", ruta_archivo_bloques);
        return;
    }
    
    for (int i = 0; i < cant_bloques; i++)
    {
        char* datos_bloque = malloc(tamano_bloque * sizeof(char));
        for (int j = 0; j < tamano_bloque; j++)
        {
            datos_bloque[j] = '0';
        }
        fwrite(datos_bloque, sizeof(char) * tamano_bloque, 1, archivo_bloques);
        free(datos_bloque);
    }
    fclose(archivo_bloques);
}


void leer_archivo_de_bloques(double cant_bloques, double tamano_bloque, char* ruta_archivo_bloques)
{
    
    FILE* archivo_bloques = fopen(ruta_archivo_bloques, "r");
    if (archivo_bloques ==  NULL) {log_info(logger, "Error abriendo el archivo %s", ruta_archivo_bloques);}
    for (int i = 0; i < cant_bloques; i++)
    {
        char* datos = malloc(tamano_bloque * sizeof(char));
        fread(datos, tamano_bloque * sizeof(char), 1, archivo_bloques);
        printf("\n%s\n", datos);
        free(datos);
    }
    fclose(archivo_bloques);
}

void leer_bloque_completo(int nro_bloque, int tamano_bloque, char* ruta_archivo_bloques)
{
    
    FILE* archivo_bloques = fopen(ruta_archivo_bloques, "r");

    if (archivo_bloques == NULL){log_info(logger, "Error abriendo el archivo %s", ruta_archivo_bloques);}

    char* datos = malloc(tamano_bloque * sizeof(char));

    int desplazamiento_total = tamano_bloque * nro_bloque;
    fseek(archivo_bloques, desplazamiento_total * sizeof(char), SEEK_SET);
    fread(datos, tamano_bloque * sizeof(char), 1, archivo_bloques);

    log_info(logger, "En el bloque %i hay %s", nro_bloque, datos);

    free(datos);
    fclose(archivo_bloques);
}

char* leer_bloque_desde_hasta(uint32_t nro_bloque, uint32_t desde, uint32_t hasta, int tamano_bloque, char* ruta_archivo_bloque)
{
    FILE* archivo_bloques = fopen(ruta_archivo_bloque, "r");

    //int desplazamiento = nro_bloque * tamano_bloque + desde;
    int bytes_a_leer = hasta - desde;
    char* datos = malloc((bytes_a_leer + 1) * sizeof(char));

    fseek(archivo_bloques, desde, SEEK_SET);
    fread(datos, sizeof(char), bytes_a_leer, archivo_bloques);
    datos[bytes_a_leer] = '\0';
    log_info(logger, "Lei %s", datos);

    return datos;
}

char* obtener_puntero_bloque_libre(int cant_bloques, t_bitarray* bitarray)
{

    int index = 0;
    while (bitarray_test_bit(bitarray, index))
    {
        index++;
    }
    char* puntero = completar_con_ceros(index, cant_bloques);
    return puntero;
}

char* completar_con_ceros(int index, int cant_bloques)
{
    char cadena_cant_bloques[20];
    sprintf(cadena_cant_bloques, "%d", cant_bloques);
    char cadena_index[20];
    sprintf(cadena_index, "%d", index);
    //index es el nro de BLOQUe
    int cantidad_de_digitos_bloques = strlen(cadena_cant_bloques);
    int cantidad_de_digitos_index = strlen(cadena_index);
    
    char* numero_bloque = malloc(cantidad_de_digitos_bloques); 
    for(int i = 0; i < (cantidad_de_digitos_bloques - cantidad_de_digitos_index); i++){
        numero_bloque[i] = '0';
    }
    int j = 0;
    for(int i = (cantidad_de_digitos_bloques - cantidad_de_digitos_index); i < cantidad_de_digitos_bloques; i++)
    {
        numero_bloque[i] = cadena_index[j];
        j++;
    }
    
    numero_bloque[cantidad_de_digitos_bloques] = '\0';
    return numero_bloque;
}

void escribir_en_archivo_de_bloques(char* ruta)
{
    FILE* archivo_de_bloque = fopen(ruta, "w");

    char* datos = "DALEEEEE FUNCIONAAAA";
    fwrite(datos, sizeof(datos), 1, archivo_de_bloque);
    fclose(archivo_de_bloque);
}