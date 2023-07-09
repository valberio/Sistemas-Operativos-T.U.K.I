#include "bloques.h"

void crear_archivo_bloques(double cant_bloques, double tamano_bloque, char* ruta_archivo_bloques)
{

    FILE* archivo_bloques = fopen(ruta_archivo_bloques, "wb");
    
    //Crear todos los bloques
    for(int i = 0; i < cant_bloques; i++)
    {
        t_bloque bloque;
        bloque.datos = malloc(tamano_bloque);
        bloque.datos = 123456;
        fwrite(&bloque, sizeof(t_bloque), 1, archivo);
        free(bloque);
    }
    //Guardar en el archivo
}

