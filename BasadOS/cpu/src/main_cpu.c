#include "main_cpu.h"



int main(void)
{

	t_log *logger = iniciar_logger("log_cpu.log", "LOG_CPU");
	t_config *config = iniciar_config("configs/cpu.config");
	int retardo_instruccion = config_get_int_value(config, "RETARDO_INSTRUCCION") / 1000;
	int tam_max_segmento = config_get_int_value(config, "TAM_MAX_SEGMENTO");

	// CPU como cliente para memoria
	int conexion_memoria_cpu = conectarse_a_memoria(config, logger);

	// CPU como server del Kernel
	int conexion_cpu_kernel = conexion_a_kernel(config, logger);
	config_destroy(config);

	if (conexion_cpu_kernel)
	{
		log_info(logger, "CPU recibió al kernel");

		while (conexion_cpu_kernel)
		{

			t_contexto_de_ejecucion *contexto = malloc(sizeof(t_contexto_de_ejecucion));
			t_paquete *paquete = malloc(sizeof(t_paquete));
			paquete = recibir_contexto_de_ejecucion(conexion_cpu_kernel);
			contexto = deserializar_contexto_de_ejecucion(paquete->buffer);

			if (contexto == NULL)
			{
				log_info(logger, "No recibi mas procesos del kernel");
				liberar_contexto_de_ejecucion(contexto);
				eliminar_paquete(paquete);
				close(conexion_cpu_kernel);
				conexion_cpu_kernel = 0;
			}
			else
			{

				int cant_instrucciones = list_size(contexto->instrucciones);

				bool cortar_ejecucion = false;

				while (contexto->program_counter < cant_instrucciones && !cortar_ejecucion) // Itero sobre todas las instrucciones, ejecutando
				{
					char *instruccion = fetch(contexto);

					char **instruccion_array = decode(instruccion, retardo_instruccion, tam_max_segmento, contexto->tabla_segmentos, logger, contexto);

					if (strcmp(instruccion_array[0], "SEGFAULT") == 0)
					{
						cortar_ejecucion = true;
						t_paquete* paquete = crear_paquete();
						paquete->codigo_operacion = SEGMENTATATION_FAULT;
						paquete->buffer = serializar_contexto(contexto);
						enviar_paquete(paquete, conexion_cpu_kernel);
						break;
					}

					contexto->program_counter++;

					int resultado = execute(logger, instruccion_array, contexto, conexion_cpu_kernel, conexion_memoria_cpu); // El envio del contexto de ejecucion al kernel pasa en execute

					// Si execute devuelve un 1, desalojo. Si devuelve un 0, no. Desalojo sacando del while
					switch (resultado)
					{
					case 1:
						cortar_ejecucion = true;
						log_info(logger, "Desalojé el proceso");

						eliminar_paquete(paquete);
						
						break;
					case 0:
						break;
					}
					free(instruccion_array);
				}
				liberar_contexto_de_ejecucion(contexto);
			}
		}
		log_destroy(logger);
	}

	// liberar_conexion(conexion_cpu_kernel);
	// liberar_conexion(conexion_memoria_cpu);

	return 0;
}

int conectarse_a_memoria(t_config *config, t_log *logger)
{
	char *ip = config_get_string_value(config, "IP_MEMORIA");
	char *puerto_memoria_cpu = config_get_string_value(config, "PUERTO_MEMORIA");

	int conexion_memoria_cpu = crear_conexion_al_server(logger, ip, puerto_memoria_cpu);

	if (conexion_memoria_cpu == -1)
	{
		log_info(logger, "Error conectandose a memoria");
	}

	return conexion_memoria_cpu;
}

int conexion_a_kernel(t_config *config, t_log *logger)
{
	char *puerto_cpu_kernel = config_get_string_value(config, "PUERTO_ESCUCHA");

	int server_para_kernel = iniciar_servidor(logger, puerto_cpu_kernel);

	int conexion_kernel = esperar_cliente(server_para_kernel);
	return conexion_kernel;
}
