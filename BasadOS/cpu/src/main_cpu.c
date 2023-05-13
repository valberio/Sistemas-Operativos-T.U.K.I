#include "main_cpu.h"




/*------------------------------------------------------------------*/
/*|						CHECKPOINT 2					   			*/
/*------------------------------------------------------------------*/
/*		Levanta el archivo de configuración: HECHO
		Se conecta a Memoria y espera conexiones del Kernel: HECHO.
		Ejecuta las instrucciones SET, YIELD y EXIT.				*/
/*------------------------------------------------------------------*/

//Debatir: que condicion de corte le pongo al while?

//TODO: Funcion que devuelva el contexto de ejecución al kernel

int main(void)
{

	t_log * logger = iniciar_logger("log_cpu.log","LOG_CPU");
	t_config* config = iniciar_config("configs/cpu.config");
	
	/*char* inst = "SET\nYIELD\nEXIT\nESTO ES DE PRUEBA";

	t_pcb* pcb = crear_pcb(inst);
	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer = serializar_contexto(pcb->contexto_de_ejecucion);
	t_contexto_de_ejecucion* con = deserializar_contexto_de_ejecucion(buffer);


	for (int i = 0; i<4; i++)
	{
		printf("%s\n", list_get(con->instrucciones, i));
	}*/

	//CPU como cliente para memoria
	//int conexion_memoria_cpu = conectarse_a_memoria(logger);

	//CPU como server del Kernel
	int conexion_cpu_kernel = conexion_a_kernel(config, logger);
	config_destroy(config);


	if(conexion_cpu_kernel) //Cuando quieran probar la conexion con kernel, pongan conexion_cpu_kernel acá
	{
		log_info(logger, "CPU recibió al kernel");
		while(conexion_cpu_kernel) {
			t_contexto_de_ejecucion* contexto = malloc(sizeof(t_contexto_de_ejecucion));
			t_paquete* paquete = malloc(sizeof(t_paquete));
			paquete = recibir_contexto_de_ejecucion(conexion_cpu_kernel); //Y descomenten esto
			contexto = deserializar_contexto_de_ejecucion(paquete->buffer);

			if(contexto == NULL) {
				log_info(logger, "error con el contexto");
				break;
				}

		    	/*for(int i = 0; i < contexto->cant_instrucciones; i++)
    			{
        		printf("Largo instruccion %i\n", contexto->largo_instruccion[i]);
    			}*/


			//contexto->program_counter = 0; //Esto hay que pasarlo a crear_pcb
			printf("\n");
			log_info(logger, "Recibi un nuevo proceso");
			int cant_instrucciones = list_size(contexto->instrucciones);
			log_info(logger, "Cantidad instrucciones: %i", cant_instrucciones);
			while (contexto->program_counter < cant_instrucciones) //Itero sobre todas las instrucciones, ejecutando
			{
				char * instruccion = fetch(contexto);
				log_info(logger, "La instruccion a ejecutar es %s", instruccion);
				
				char* *instruccion_array = decode(instruccion);
				
				execute(logger, instruccion_array, contexto, conexion_cpu_kernel); //El envio del contexto de ejecucion al kernel pasa en execute

				contexto->program_counter++;
			}
			liberar_contexto_de_ejecucion(contexto);
		}
		log_destroy(logger);
	}
	close(conexion_cpu_kernel);
	//liberar_conexion(&conexion_cpu_kernel);
	return 0;
}

int conectarse_a_memoria(t_config* config, t_log* logger)
{
	char* ip = config_get_string_value(config, "IP");
	char* puerto_memoria_cpu = config_get_string_value(config, "PUERTO_MEMORIA");

	int conexion_memoria_cpu = crear_conexion_al_server(logger, ip, puerto_memoria_cpu);

	if (conexion_memoria_cpu == -1)
	{
		log_info(logger, "Error conectandose a memoria");
	}
	
	return conexion_memoria_cpu;
}

int conexion_a_kernel(t_config* config,t_log* logger)
{
	char* puerto_cpu_kernel =  config_get_string_value(config, "PUERTO_KERNEL");
	char* ip = config_get_string_value(config, "IP");

	int server_para_kernel = iniciar_servidor(logger, ip, puerto_cpu_kernel);

	int conexion_kernel = esperar_cliente(server_para_kernel);
	return conexion_kernel;
}



// void devolver_contexto(t_contexto_de_ejecucion* contexto, int conexion_cpu_kernel)
// {
// 	//TODO
// 	//A saber: la CPU *siempre* le termina devolviendo el contexto de ejecucion al kernel,
// 	//porque siempre las instrucciones terminan con EXIT.
// 	
// }

//Si execute() devuelve un 1, es porque la instruccion pide que se devuelva el contexto
//al kernel. 

//DUDA: "Para las siguientes instrucciones se deberá devolver al módulo Kernel
// el Contexto de Ejecución actualizado junto al motivo del desalojo"
//Cuáles son los motivos de desalojo?


