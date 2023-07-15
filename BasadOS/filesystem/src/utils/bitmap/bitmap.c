#include "bitmap.h"




t_bitarray *crear_bitmap(char *ruta, double block_count)
{
    
    t_bitarray* bitarray;
    if (access(ruta, F_OK) != -1) // Si el archivo existe, lo abro como bitarray
    {
        char *bitmap = malloc((block_count / 8));
        bitarray = bitarray_create_with_mode(bitmap, sizeof(bitmap), LSB_FIRST);
        for (int i = 0; i < block_count; i++)
        {
            bitarray_clean_bit(bitarray, i);
        }
    }
    else  //Si no existe, lo creo e inicializo en 0
    {
        FILE *archivo_bitmap = fopen(ruta, "w");
        if (archivo_bitmap == NULL)
        {
            log_info(logger, "Error creando el archivo bitmap");
        }

        char *bitmap = malloc((block_count / 8));
        bitarray = bitarray_create_with_mode(bitmap, sizeof(bitmap), LSB_FIRST);

        for (int i = 0; i < block_count; i++)
        {
            bitarray_clean_bit(bitarray, i);
            if (fseek(archivo_bitmap, i, SEEK_SET) != 0)
            {
                log_info(logger, "Error al mover el puntero de archivo.\n");
            }
            int valor = 0;
            if (fwrite(&valor, sizeof(int), 1, archivo_bitmap) != 1)
            {
                log_info(logger, "Error al escribir en el archivo.\n");
            }
        }
        fclose(archivo_bitmap);
    }
    return bitarray;
}

void setear_bit(int index)
{
    FILE *bitmap = fopen(ruta_bitmap, "rb+");
    bitarray_set_bit(bitarray, index);
    fseek(bitmap, index, SEEK_SET);
    char bit = '1';
    fwrite(&bit, sizeof(char), 1, bitmap);
    fclose(bitmap);
    log_info(logger, "Acceso a Bitmap - Bloque: %i - Estado: 1", index);
}

void limpiar_bit(int index)
{
    FILE *bitmap = fopen(ruta_bitmap, "rb+");
    bitarray_clean_bit(bitarray, index);
    fseek(bitmap, index, SEEK_SET);
    char bit = '0';
    fwrite(&bit, sizeof(char), 1, bitmap);
    fclose(bitmap);
    log_info(logger, "Acceso a Bitmap - Bloque: %i - Estado: 0", index);
}

char ver_bit(int index)
{
    FILE *bitmap = fopen(ruta_bitmap, "rb");
    fseek(bitmap, index, SEEK_SET);
    char bit;
    fread(&bit, sizeof(char), 1, bitmap);
    fclose(bitmap);
    return bit;
}