#include "main.h"
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>

#include "/home/utnso/tp-2023-1c-BasadOS/BasadOS/utils/src/sockets/socketUtils.c"


int main(int argc, char* argv[]) {

    t_log* logger = log_create("log_memoria.log", "Servidor", 1, LOG_LEVEL_DEBUG);

	//La memoria tiene en paralelo 3 conexiones: con kernel, cpu, y fileSystem

	//Creo el server de la memoria en esta ip y puerto
	char* ip = "127.0.0.1";

	char* puerto_kernel = "4444";
    char* puerto_cpu = "35789";
    char* puerto_filesystem = "4445";

	int servidor_memoria_kernel = iniciar_servidor(logger, ip, puerto_kernel);
    int servidor_memoria_cpu = iniciar_servidor(logger, ip, puerto_cpu);
    int servidor_memoria_filesystem = iniciar_servidor(logger, ip, puerto_filesystem);
	//Guardo las conexiones con cada modulo en un socket distinto,
	//cada módulo se conecta a través de un puerto diferente.
   
   int conexion_filesystem = esperar_cliente(logger, servidor_memoria_filesystem);
   if (conexion_filesystem)
   {
		log_info(logger, "Se conectó el fileSystem");
   }

    int conexion_cpu = esperar_cliente(logger, servidor_memoria_cpu);
   if (conexion_cpu)
   {
		log_info(logger, "Se conectó la CPU");
   }

   int conexion_kernel = esperar_cliente(logger, servidor_memoria_kernel);
   if (conexion_kernel)
   {
		log_info(logger, "Se conecto el kernel");
   }

   if (conexion_kernel == -1)
   {
		log_info(logger, "Error conectando el kernel");
		return 0;
   }
    

   

}

/*void iterator(char* value) {
	log_info(logger,"%s", value);
}*/
