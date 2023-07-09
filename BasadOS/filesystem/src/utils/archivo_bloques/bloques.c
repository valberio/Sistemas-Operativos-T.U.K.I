#include "bloques.h"

void crear_archivo_bloques(double cant_bloques, double tamano_bloque, char* ruta_archivo_bloques)
{

    FILE* archivo_bloques = fopen("bloques.txt", "w");
    if(archivo_bloques == NULL)
    {
        printf("error abriendo el archivo");
        return -1;
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
    FILE* archivo_bloques = fopen("bloques.txt", "r");

    for (int i = 0; i < cant_bloques; i++)
    {
        char* datos = malloc(tamano_bloque * sizeof(char));
        fread(datos, tamano_bloque * sizeof(char), 1, archivo_bloques);
        printf("\n%s\n", datos);
        free(datos);
    }
}