#include "main.h"

int main(int argc, char* argv[]) {
    logger = log_create("logs/log_kernel.log", "Servidor", 1, LOG_LEVEL_DEBUG);

    int server = iniciar_servidor();
	log_info(logger, "Servidor listo para recibir al cliente");
    int client = esperar_cliente(server);

    t_list* lista;
    while (1) {
		int cod_op = recibir_operacion(client);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(client);
			break;
		case PAQUETE:
			lista = recibir_paquete(client);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void*) iterator);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
    log_destroy(logger);
    
    return 0;
}

void iterator(char* value) {
	log_info(logger,"%s", value);
}
