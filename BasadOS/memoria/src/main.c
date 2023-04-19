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
	char* puerto = "35820";

	int servidor_memoria = iniciar_servidor(logger, ip, puerto);


	//Guardo las conexiones con cada modulo en un socket distinto,
	//cada módulo se conecta a través de un puerto diferente.
   
   int conexion_kernel = esperar_cliente(logger, servidor_memoria);
   if (conexion_kernel)
   {
		log_info(logger, "Se conecto la kernel");
   }

   if (conexion_kernel == -1)
   {
		log_info(logger, "Error conectando la kernel");
		return 0;
   }

   int conexion_filesystem = esperar_cliente(logger, servidor_memoria);
   if (conexion_filesystem)
   {
		log_info(logger, "Se conectó el fileSystem");
   }
   
   int conexion_cpu = esperar_cliente(logger, servidor_memoria);
   if (conexion_cpu)
   {
		log_info(logger, "Se conectó la CPU");
   }
}

/*void iterator(char* value) {
	log_info(logger,"%s", value);
}*/
