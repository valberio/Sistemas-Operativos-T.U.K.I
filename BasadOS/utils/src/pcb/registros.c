#include "registros.h"



enum Registros string_a_registro(char *registro)
{
    if (strncmp(registro, "AX", 2) == 0)
    {
        return rAX;
    }
    if (strncmp(registro, "BX", 2) == 0)
    {
        return rBX;
    }
    if (strncmp(registro, "CX", 2) == 0)
    {
        return rCX;
    }
    if (strncmp(registro, "DX", 2) == 0)
    {
        return rDX;
    }

    // Registros de 8 bits
    if (strncmp(registro, "EAX", 3) == 0)
    {
        return rEAX;
    }
    if (strncmp(registro, "EBX", 3) == 0)
    {
        return rEBX;
    }
    if (strncmp(registro, "ECX", 3) == 0)
    {
        return rECX;
    }
    if (strncmp(registro, "EDX", 3) == 0)
    {
        return rEDX;
    }

    // Registros de 16 bits
    if (strncmp(registro, "RAX", 3) == 0)
    {
        return rRAX;
    }
    if (strncmp(registro, "RBX", 3) == 0)
    {
        return rRBX;
    }
    if (strncmp(registro, "RCX", 3) == 0)
    {
        return rRCX;
    }
    if (strncmp(registro, "RDX", 3) == 0)
    {
        return rRDX;
    }
    return EXIT_FAILURE;
}


char* leer_registro(char *registro_char, t_registros* registros)
{
    enum Registros registro = string_a_registro(registro_char);
    int tamanio_registro = tamanio_del_registro(registro_char);
    char *valor_en_registro = malloc(tamanio_registro * sizeof(char));

    switch (registro)
    {
    case rAX:
        strcpy(valor_en_registro, registros->AX);
        break;
    case rBX:
        strcpy(valor_en_registro, registros->BX);
        break;
    case rCX:
        strcpy(valor_en_registro, registros->CX);
        break;
    case rDX:
        strcpy(valor_en_registro, registros->DX);
        break;

    // Registros 8 bits
    case rEAX:
        strcpy(valor_en_registro, registros->EAX);
        break;

    case rEBX:
        strcpy(valor_en_registro, registros->EBX);
        break;

    case rECX:
        strcpy(valor_en_registro, registros->ECX);
        break;

    case rEDX:
        strcpy(valor_en_registro, registros->EDX);
        break;

    // Registros de 16 bits
    case rRAX:
        strcpy(valor_en_registro, registros->RAX);
        break;

    case rRBX:
        strcpy(valor_en_registro, registros->RBX);
        break;

    case rRCX:
        strcpy(valor_en_registro, registros->RCX);
        break;

    case rRDX:
        strcpy(valor_en_registro, registros->RDX);
        break;
    }
    return valor_en_registro;
}


int tamanio_del_registro(char *registro_char)

{
    int tamanio_del_registro = 0;
    enum Registros registro = string_a_registro(registro_char);
    switch (registro)
    {
    case rAX:
        tamanio_del_registro = 5; // 4 + 1, para considerar el \0
        break;
    case rBX:
        tamanio_del_registro = 5;
        break;
    case rCX:
        tamanio_del_registro = 5;
        break;
    case rDX:
        tamanio_del_registro = 5;
        break;

    // Registros 8 bits
    case rEAX:
        tamanio_del_registro = 9;
        break;

    case rEBX:
        tamanio_del_registro = 9;
        break;

    case rECX:
        tamanio_del_registro = 9;
        break;

    case rEDX:
        tamanio_del_registro = 9;
        break;

    // Registros de 16 bits
    case rRAX:
        tamanio_del_registro = 17;
        break;

    case rRBX:
        tamanio_del_registro = 17;
        break;

    case rRCX:
        tamanio_del_registro = 17;
        break;

    case rRDX:
        tamanio_del_registro = 17;
        break;
    }
    return tamanio_del_registro;
}


void guardar_en_registros(char *registro_char, char *datos, t_registros* registros) 
{
    enum Registros registro = string_a_registro(registro_char);

    switch (registro)
    {
    case rAX:
        strcpy(registros->AX, datos);
        break;
    case rBX:
        strcpy(registros->BX, datos);
        break;
    case rCX:
        strcpy(registros->CX, datos);
        break;
    case rDX:
        strcpy(registros->DX, datos);
        break;

    // Registros 8 bits
    case rEAX:
        strcpy(registros->EAX, datos);
        break;

    case rEBX:
        strcpy(registros->EBX, datos);
        break;

    case rECX:
        strcpy(registros->ECX, datos);
        break;

    case rEDX:
        strcpy(registros->EDX, datos);
        break;

    // Registros de 16 bits
    case rRAX:
        strcpy(registros->RAX, datos);
        break;

    case rRBX:
        strcpy(registros->RBX, datos);
        break;

    case rRCX:
        strcpy(registros->RCX, datos);
        break;

    case rRDX:
        strcpy(registros->RDX, datos);
        break;
    }
}