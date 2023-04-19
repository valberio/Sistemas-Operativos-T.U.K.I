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

	printf("asfgnjalkdfnga");

    t_log* logger = log_create("log_memoria.log", "Servidor", 1, LOG_LEVEL_DEBUG);
	log_info(logger, "Hoooolaaaa");

	//La memoria tiene en paralelo 3 conexiones: con kernel, cpu, y fileSystem

	//Creo el server de la memoria en esta ip y puerto
	char* ip = "127.0.0.1";
	char* puerto = "4040";

	int servidor_memoria = iniciar_servidor(logger, ip, puerto);


	//Guardo las conexiones con cada modulo en un socket distinto,
	//cada módulo se conecta a través de un puerto diferente.
   
   int conexion_kernel = esperar_cliente(logger, servidor_memoria);

   if (conexion_kernel == -1)
   {
		return 0;
   }
}

/*void iterator(char* value) {
	log_info(logger,"%s", value);
}*/
