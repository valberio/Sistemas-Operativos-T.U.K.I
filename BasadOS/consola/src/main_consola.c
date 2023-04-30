#include "main_consola.h"

/*                  CHECKPOINT 2                        */
/*------------------------------------------------------*/
/*  Levanta el archivo de configuración: HECHO! 
    Levanta e interpreta el archivo de pseudocódigo: HECHO!!
    Se conecta al Kernel y envía las instrucciones: HECHO!!!! */
/*------------------------------------------------------*/  


int main(int argc, char* argv[]) {
    printf("%d", argc);
    if(argc < 3){
        printf("holi");
        return EXIT_FAILURE;
    }

    // Lectura e impresion de pseudocodigo
    t_config* config = iniciar_config(argv[1]);

    // Lectura de config
    char *pseudocodigo = leer_Pseudocodigo(argv[2]);


    char* ip = config_get_string_value(config, "IP");
    char* puerto_kernel_consola = config_get_string_value(config, "PUERTO_KERNEL");

    int conexion_con_kernel = levantar_conexion(ip, puerto_kernel_consola);
    
    
    enviar_mensaje(pseudocodigo,conexion_con_kernel);    
    free(pseudocodigo);

}

int levantar_conexion(char* ip, char* puerto_kernel_consola) {
 
    t_log* logger_consola = iniciar_logger("log_consola.log","LOG_CONSOLA");
    
    int conexion_kernel = crear_conexion_al_server(logger_consola, ip, puerto_kernel_consola);
    if (conexion_kernel)
    {
        log_info(logger_consola, "Consola envió su conexión al kernel");
        return conexion_kernel;
    }
    if ((conexion_kernel == -1))
    {
        log_info(logger_consola, "Error conectando la consola con el kernel");
        return -1;
    }
   return 0;
}

char *leer_Pseudocodigo(char *archivo_path) {
    char *buffer = NULL;
    int string_size, read_size;
    FILE *handler = fopen(archivo_path, "r");

    if (handler) {
        // Se posiciona el puntero del archivo al final del archivo para obtener su tamaño
        fseek(handler, 0, SEEK_END);
        // Se obtiene el tamaño del archivo
        string_size = ftell(handler);
        // Se reposiciona el puntero del archivo al principio del archivo
        rewind(handler);

        // Se asigna memoria dinámica para almacenar el contenido del archivo
        buffer = (char*) malloc(sizeof(char) * (string_size + 1));

        // Se lee el contenido del archivo en el buffer
        read_size = fread(buffer, sizeof(char), string_size, handler);

        // Se añade el caracter nulo para finalizar la cadena de caracteres
        buffer[string_size] = '\0';

        if (read_size != string_size) {
            // Si el tamaño leído no coincide con el tamaño del archivo, se libera la memoria asignada
            free(buffer);
            buffer = NULL;
        }

        // Se cierra el archivo
        fclose(handler);
    }

    return buffer;
}
