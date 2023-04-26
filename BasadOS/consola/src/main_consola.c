#include "main_consola.h"



int main(int argc, char* argv[]) {
    printf("%d", argc);
    if(argc < 3){
        printf("holi");
        return EXIT_FAILURE;
    }
    t_config* config = iniciar_config(argv[1]);
    //FILE *pesudocodigo = fopen(argv[2],"r");
    t_paquete* paquete = crear_paquete();
    char* ip = config_get_string_value(config, "IP");
    char* puerto_kernel_consola = config_get_string_value(config, "PUERTO_KERNEL");

    int conexion_con_kernel = levantar_conexion(ip, puerto_kernel_consola);

    char* instruccion1 = "SET";
    char* instruccion2 = "MOV";
    crear_buffer(paquete);
    agregar_a_paquete(paquete,instruccion1,sizeof(instruccion1)+1);
    agregar_a_paquete(paquete,instruccion2,sizeof(instruccion1)+1);
    enviar_paquete(paquete,conexion_con_kernel);
    eliminar_paquete(paquete):
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


