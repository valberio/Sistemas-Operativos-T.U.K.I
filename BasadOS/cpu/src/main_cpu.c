#include "main_cpu.h"

/*------------------------------------------------------------------*/
/*|						CHECKPOINT 3					   			*/
/*------------------------------------------------------------------*/
/*		Interpreta todas las operaciones
		Ejecuta las instrucciones I/O, WAIT y SIGNAL.				*/
/*------------------------------------------------------------------*/

// TO-DO GENERAL CPU
/*-Serializar los distintos valores de retorno de las instrucciones (parametros
	para kernel)
  -Implementar de manera genérica todas las instrucciones
  -Serializar diccionarios y cambiar el contexto a un diccionario
	*/

int main(void)
{

	t_log *logger = iniciar_logger("log_cpu.log", "LOG_CPU");
	t_config *config = iniciar_config("configs/cpu.config");
	int retardo_instruccion = config_get_int_value(config, "RETARDO_INSTRUCCION");

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
				close(conexion_cpu_kernel);
				conexion_cpu_kernel = 0;
			}
			else
			{

				int cant_instrucciones = list_size(contexto->instrucciones);

				while (contexto->program_counter < cant_instrucciones) // Itero sobre todas las instrucciones, ejecutando
				{
					char *instruccion = fetch(contexto);

					char **instruccion_array = decode(instruccion, retardo_instruccion);

					contexto->program_counter++;

					int resultado = execute(logger, instruccion_array, contexto, conexion_cpu_kernel, conexion_memoria_cpu); // El envio del contexto de ejecucion al kernel pasa en execute

					// Si execute devuelve un 1, desalojo. Si devuelve un 0, no. Desalojo sacando del while
					switch (resultado)
					{
					case 1:
						goto desalojo;
						break;
					case 0:
						break;
					}
					free(instruccion_array);
				}

			desalojo:
				log_info(logger, "Desalojé el proceso");

				eliminar_paquete(paquete);
				liberar_contexto_de_ejecucion(contexto);
			}
		}
		log_destroy(logger);
	}

	// liberar_conexion(&conexion_cpu_kernel);
	return 0;
}

int conectarse_a_memoria(t_config *config, t_log *logger)
{
	char *ip = config_get_string_value(config, "IP");
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
	char *puerto_cpu_kernel = config_get_string_value(config, "PUERTO_KERNEL");
	char *ip = config_get_string_value(config, "IP");

	int server_para_kernel = iniciar_servidor(logger, ip, puerto_cpu_kernel);

	int conexion_kernel = esperar_cliente(server_para_kernel);
	return conexion_kernel;
}
