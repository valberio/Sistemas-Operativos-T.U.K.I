#include "bloques.h"

void vaciar_archivo_bloques(double cant_bloques, double tamano_bloque)
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
        char* datos_bloque = malloc(tamano_bloque * sizeof(int));
        for (int j = 0; j < tamano_bloque; j++)
        {
            datos_bloque[j] = 0;
        }
        fwrite(datos_bloque, sizeof(int), tamano_bloque, archivo_bloques);
        free(datos_bloque);
    }
    fclose(archivo_bloques);
}


void leer_archivo_de_bloques(double cant_bloques, double tamano_bloque)
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

void leer_bloque_completo(int nro_bloque, int tamano_bloque)
{
    
    FILE* archivo_bloques = fopen(ruta_archivo_bloques, "r");

    if (archivo_bloques == NULL){log_info(logger, "Error abriendo el archivo %s", ruta_archivo_bloques);}

    char* datos = malloc(tamano_bloque * sizeof(char));

    int desplazamiento_total = tamano_bloque * nro_bloque;
    fseek(archivo_bloques, desplazamiento_total * sizeof(char), SEEK_SET);
    fread(datos, tamano_bloque * sizeof(char), 1, archivo_bloques);

    datos[tamano_bloque] = '\0';

    free(datos);
    fclose(archivo_bloques);
}

char* leer_bloque_desde_hasta(uint32_t nro_bloque, uint32_t desde, uint32_t hasta, int tamano_bloque)
{
    FILE* archivo_bloques = fopen(ruta_archivo_bloques, "r");

    //int desplazamiento = nro_bloque * tamano_bloque + desde;
    int bytes_a_leer = hasta - desde;
    char* datos = malloc((bytes_a_leer + 1) * sizeof(char));

    fseek(archivo_bloques, desde, SEEK_SET);
    fread(datos, sizeof(char), bytes_a_leer, archivo_bloques);
    datos[bytes_a_leer] = '\0';

    return datos;
}

int obtener_puntero_bloque_libre(int cant_bloques)
{
    int index = 0;
    while (bitarray_test_bit(bitarray, index))
    {
        index++;
    }
    
    return index;
}
