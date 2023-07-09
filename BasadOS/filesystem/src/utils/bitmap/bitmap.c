#include "bitmap.h"


 t_bitarray* crear_bitmap (char* ruta, double block_count)
 {
    FILE* archivo_bitmap = fopen(ruta, "w");
    char* bitmap = malloc((block_count / 8));
	t_bitarray *bitarray = bitarray_create_with_mode(bitmap, sizeof(bitmap), LSB_FIRST);

    for(int i = 0; i < block_count/8; i++)
    {
        bitarray_clean_bit(bitarray, i);
        if (fseek(archivo_bitmap, i, SEEK_SET) != 0) {
            printf("Error al mover el puntero de archivo.\n");
        }
        int valor = 0;
        if (fwrite(&valor, sizeof(int), 1, archivo_bitmap) != 1) {
            printf("Error al escribir en el archivo.\n");
        }
    }
    fclose(archivo_bitmap);
    return bitarray;
 }

 