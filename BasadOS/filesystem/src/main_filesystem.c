#include "main.h"



/*------------------------------------------------------------------*/
/*						CHECKPOINT 3								*/
/*------------------------------------------------------------------*/
/*		Espera las peticiones del Kernel y responde las mismas
		con mensajes genéricos.
		Levanta los archivos de Superbloque, bitmap y 
		archivo de bloques.
																	*/
/*------------------------------------------------------------------*/					



int main(void)
{
	t_log * logger = iniciar_logger("log_filesystem.log", "LOG_FILESYSTEM");
	t_config* config = iniciar_config("configs/filesystem.config");

	t_superbloque superbloque;
	t_config* superbloque_config = iniciar_config("configs/superbloque.config");
	superbloque.block_size = config_get_string_value(superbloque_config, "BLOCK_SIZE");
	superbloque.block_count = config_get_string_value(superbloque_config, "BLOCK_COUNT");

	void* bitmap = malloc((int)ceil(superbloque.block_count / 8.0));
	t_bitarray* bitarray = bitarray_create(bitmap, sizeof(bitmap));

	char* ip = config_get_string_value(config, "IP");

	//Conecto filesystem como cliente a memoria
	char* puerto_a_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
	int cliente_filesystem_a_memoria = crear_conexion_al_server(logger, ip, puerto_a_memoria);
	if (cliente_filesystem_a_memoria)
	{
		log_info(logger, "Filesystem se conectó a memoria!");
		t_contexto_de_ejecucion* contexto = malloc(sizeof(t_contexto_de_ejecucion));
		contexto = recibir_contexto(cliente_filesystem_a_memoria);
		printf("Recibi instruccion %s, %s\n", list_get(contexto->instrucciones, 0), list_get(contexto->instrucciones, 1));
	}
}

	//Conecto el filesystem como servidor del kernel
	/*char* puerto_a_kernel = config_get_string_value(config, "PUERTO_KERNEL");
	int servidor_filesystem = iniciar_servidor(logger, ip, puerto_a_kernel);
	int conexion_filesystem_kernel = esperar_cliente(logger, servidor_filesystem);

	if (conexion_filesystem_kernel)
	{
		log_info(logger, "Filesystem recibió la conexión del kernel!");
	}
}*/

int abrir_archivo(char* nombre_archivo, t_superbloque* superbloque, t_bitarray* bitmap){
	char* temp_nombre;
	strcpy(temp_nombre, "../files/");
	strcat(temp_nombre, nombre_archivo);
	FILE* archivo = fopen(temp_nombre,"r");
	if(archivo != NULL){
		return 0;
	}
	else{
		log_info(logger, "No existe el archivo al que se intenta acceder.");
	}
}

int crear_archivo(char* nombre_archivo, t_superbloque* superbloque, t_bitarray* bitmap){
	char* ruta;
	strcpy(ruta, "../files/");
	strcat(ruta, nombre_archivo);
	FILE* archivo = fopen(ruta,"w");
	if(archivo == NULL){
		log_info(logger, "Error al crear el archivo.");
	}
	crear_fcb(nombre_archivo);
	free(ruta);
	return 0;
}
void* crear_fcb(char* nombre_archivo){
	char* ruta;
	strcpy(ruta, "../files/");
	strcat(ruta, nombre_archivo);
	t_config* fcb = config_create(ruta);
	config_set_value(fcb, "NOMBRE_ARCHIVO", nombre_archivo);
	config_set_value(fcb, "TAMANIO_ARCHIVO", "0");
	config_set_value(fcb, "PUNTERO_DIRECTO", "0");
	config_set_value(fcb, "PUNTERO_INDIRECTO", "0");
	config_save(fcb);
	free(ruta);
	return 0;
}