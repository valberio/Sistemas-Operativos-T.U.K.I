#include "bloques.h"

uint32_t obtener_puntero_bloque_libre()
{
    uint32_t index = 0;
    while (bitarray_test_bit(bitarray, index))
    {
        index++;
    }
    
    return index;
}
