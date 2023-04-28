#include "main_consola.h"

/*                  CHECKPOINT 2                        */
/*------------------------------------------------------*/
/*  Levanta el archivo de configuración: HECHO! 
    Levanta e interpreta el archivo de pseudocódigo: wip
    Se conecta al Kernel y envía las instrucciones: wip */
/*------------------------------------------------------*/  


int main(int argc, char* argv[]) {
    // printf("%d", argc);
    // if(argc < 3){
    //     printf("holi");
    //     return EXIT_FAILURE;
    // }

    // Lectura e impresion de pseudocodigo
    char *pseudocodigo = leer_Pseudocodigo("pseudocodigo.txt");
    printf("%s", pseudocodigo);
    free(pseudocodigo);

    //t_config* config = iniciar_config(argv[1]);
    //FILE *pesudocodigo = fopen(argv[2],"r");
    t_config* config = config_create("/home/utnso/tp-2023-1c-BasadOS/BasadOS/consola/configs/consola.config");


    char* ip = config_get_string_value(config, "IP");
    char* puerto_kernel_consola = config_get_string_value(config, "PUERTO_KERNEL");

    int conexion_con_kernel = levantar_conexion(ip, puerto_kernel_consola);

    char* instruccion1 = "SET";
    // char* instruccion2 = "MOV";
    
    t_paquete* paquete = crear_paquete();
    //crear_buffer(paquete);
    agregar_a_paquete(paquete, instruccion1, strlen(instruccion1)+1);
    // agregar_a_paquete(paquete,instruccion2,strlen(instruccion1)+1);
    enviar_paquete(paquete,conexion_con_kernel);
    eliminar_paquete(paquete);
    //void enviarMensaje(instruccion1, conexion_con_kernel);
}
/*void paquete(int conexion)
{
	// Ahora toca lo divertido!
	t_paquete* paquete;
	paquete = crear_paquete();
	// Leemos y esta vez agregamos las lineas al paquete
	while(1){
			printf("Introduce data a enviar\n");
			char* loQuePongas = readline(">");
			if(loQuePongas[0] == '\0'){
				break;
				}
			agregar_a_paquete(paquete, loQuePongas, strlen(loQuePongas) + 1);
			free(loQuePongas);
			}
	enviar_paquete(paquete, conexion);
	eliminar_paquete(paquete);
	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
	
}
*/


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
