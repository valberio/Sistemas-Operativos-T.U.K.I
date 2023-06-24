#include "main_filesystem.h"

/*------------------------------------------------------------------*/
/*						CHECKPOINT 3								*/
/*------------------------------------------------------------------*/
/*		Espera las peticiones del Kernel y responde las mismas
		con mensajes genéricos.
		Levanta los archivos de Superbloque, bitmap y
		archivo de bloques.
																	*/
/*------------------------------------------------------------------*/


t_list* fcb_list;
t_config *config;
t_log* logger;


int main()
{
	logger = iniciar_logger("log_filesystem.log", "LOG_FILESYSTEM");
	config = iniciar_config("configs/filesystem.config");
	fcb_list = list_create();
	// Conecto filesystem como cliente a memoria
	char *puerto_a_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
	char *ip = config_get_string_value(config, "IP_MEMORIA");
	int cliente_filesystem_a_memoria = crear_conexion_al_server(logger, ip, puerto_a_memoria);

	//Levanto el bitmap de bloques
	t_superbloque superbloque;
	t_config *superbloque_config = iniciar_config(config_get_string_value(config, "PATH_SUPERBLOQUE"));
	superbloque.block_size = config_get_double_value(superbloque_config, "BLOCK_SIZE");
	superbloque.block_count = config_get_double_value(superbloque_config, "BLOCK_COUNT");
	void *bitmap = malloc((superbloque.block_count / 8));
	t_bitarray *bitarray = bitarray_create(bitmap, sizeof(bitmap));

	//Recorro el directorio de FCBs y creo estructuras
	crear_estructuras_fcb();
	log_info(logger, "La cantidad de FCBs es de %d\n", list_size(fcb_list));
	log_info(logger, "El nombre del primer archivo FCB es: %s", ((t_fcb*)list_get(fcb_list, 0))->name);
	

	// Conecto el filesystem como servidor del kernel
	char* puerto_a_kernel = config_get_string_value(config, "PUERTO_ESCUCHA");
	int servidor_filesystem = iniciar_servidor(logger, puerto_a_kernel);
	int conexion_filesystem_kernel = esperar_cliente(servidor_filesystem);
	if (conexion_filesystem_kernel)
	{
	log_info(logger, "Filesystem recibió la conexión del kernel!");
	}
	
	recibir_ordenes_kernel(conexion_filesystem_kernel);
	/*if (cliente_filesystem_a_memoria)
	{
		log_info(logger, "Filesystem se conectó a memoria!");
		t_contexto_de_ejecucion *contexto = malloc(sizeof(t_contexto_de_ejecucion));
		contexto = recibir_contexto_de_ejecucion(cliente_filesystem_a_memoria);
		printf("Recibi instruccion %s, %s\n", list_get(contexto->instrucciones, 0), list_get(contexto->instrucciones, 1));
	}*/
}

void recibir_ordenes_kernel(int conexion_filesystem_kernel){
	while(conexion_filesystem_kernel){
		t_paquete* operacion = recibir_contexto_de_ejecucion(conexion_filesystem_kernel);
    	switch(operacion->codigo_operacion){
			case ABRIR_ARCHIVO:
				char* nombre_archivo = recibir_mensaje(conexion_filesystem_kernel);
				abrir_archivo(nombre_archivo, conexion_filesystem_kernel);
				break;
			default:
				break;
		}
	}
}

void crear_estructuras_fcb() 
{	
	char* directorio_fcb = config_get_string_value(config, "PATH_FCB");
	DIR *dir = opendir(directorio_fcb);
	struct dirent *ent;
    struct stat file_info;
	if(dir != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			//omite los directorios . y ..
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
                continue;
            }

			char* ruta_fcb = (char *)malloc(strlen(directorio_fcb) + strlen(ent->d_name) + 2); // +2 para / y \0
            sprintf(ruta_fcb, "%s/%s", directorio_fcb, ent->d_name);
            
            // Obtener información del archivo
            if (stat(ruta_fcb, &file_info) == -1) {
                log_info(logger, "Error al obtener información del archivo: %s\n", ent->d_name);
                free(ruta_fcb);
                continue;
            }
			printf("Archivo: %s\n", ruta_fcb);
			crear_fcb(ruta_fcb);
			free(ruta_fcb);
		}
		closedir(dir);	
	} else {
        // Error al abrir el directorio
        printf("No se pudo abrir el directorio: %s\n", directorio_fcb);
	}
}


void crear_fcb(char* ruta) //habria que llamarlo crear fcb
{
	t_fcb* fcb = malloc(sizeof(t_fcb));
	t_config *fcb_config = config_create(ruta);
	fcb->direct_pointer = config_get_int_value(fcb_config, "PUNTERO_DIRECTO");
	fcb->indirect_pointer = config_get_int_value(fcb_config, "PUNTERO_INDIRECTO");
	fcb->size = config_get_int_value(fcb_config, "TAMANIO_ARCHIVO");
	fcb->name = malloc(sizeof(config_get_string_value(fcb_config, "NOMBRE_ARCHIVO")) + 2);
	strcpy(fcb->name, config_get_string_value(fcb_config, "NOMBRE_ARCHIVO"));
	list_add(fcb_list, fcb);
}


void abrir_archivo(char *nombre_archivo, int conexion_filesystem_kernel)
{
	t_list* lista_con_archivo = list_create();
	bool existe_el_archivo(void* elemento){
		t_fcb* archivo_a_abrir;
		archivo_a_abrir = elemento;

		return strcmp(archivo_a_abrir->name, nombre_archivo) == 0;
	}
	lista_con_archivo = list_filter(fcb_list, existe_el_archivo);
	if (list_size(lista_con_archivo) == 1)
	{	
		log_info(logger, "Abrir Archivo: %s", nombre_archivo);
	}
	else
	{
		//HAY QUE CREAR EL ARCHIVO COMO LO HABIAS PENSADO NACHITOOO
		log_info(logger, "No existe el archivo al que se intenta acceder.");
	}
}
/*
int crear_archivo(char *nombre_archivo, t_superbloque *superbloque, t_bitarray *bitarray)
{
	char *ruta;
	strcpy(ruta, "../files/");
	strcat(ruta, nombre_archivo);
	strcat(ruta, ".config");
	FILE *archivo = fopen(ruta, "wb");
	if (archivo == NULL)
	{
		log_info(logger, "Error al crear el archivo.");
	}
	crear_fcb(nombre_archivo);
	free(ruta);
	return 0;
}

void *crear_fcb(char *nombre_archivo)
{
	char *ruta;
	strcpy(ruta, "../files/");
	strcat(ruta, nombre_archivo);
	strcat(ruta, ".config");
	t_config *fcb = config_create(ruta);
	config_set_value(fcb, "NOMBRE_ARCHIVO", nombre_archivo);
	config_set_value(fcb, "TAMANIO_ARCHIVO", "0");
	config_set_value(fcb, "PUNTERO_DIRECTO", "0");
	config_set_value(fcb, "PUNTERO_INDIRECTO", "0");
	config_save(fcb);
	free(ruta);
	return 0;
}


int truncar_archivo(char *nombre_archivo, int nro_bloques, t_superbloque superbloque, t_bitarray *bitarray, int bloques, t_log *logger, FILE* archivo_de_bloques)
{
Al momento de truncar un archivo, pueden ocurrir 2 situaciones:
Ampliar el tamaño del archivo: Al momento de ampliar el tamaño del archivo deberá actualizar el tamaño del archivo en el FCB y se le deberán asignar tantos bloques como sea necesario para poder direccionar el nuevo tamaño.
Reducir el tamaño del archivo: Se deberá asignar el nuevo tamaño del archivo en el FCB y se deberán marcar como libres todos los bloques que ya no sean necesarios para direccionar el tamaño del archivo (descartando desde el final del archivo hacia el principio).

	char *ruta;
	t_fcb *fcb = malloc(sizeof(t_fcb));
	unsigned bloques_restantes;
	strcpy(ruta, "../files/");
	strcat(ruta, nombre_archivo);
	strcat(ruta, ".config");
	FILE *archivo_fcb = fopen(ruta, "wr+");  
	if (abrir_archivo(nombre_archivo, superbloque, bitarray))
	{
		*fcb = leer_fcb(nombre_archivo);
		if (fcb->direct_pointer == 0 && bloques <= 0)
		{
			log_info(logger, "No se puede truncar el archivo.");
		}
		else if (bloques >= 0)
		{ // Ampliar el tamaño del archivo
			for (int i = 0; i < bloques; i++)
			{
				asignar_bloque(fcb, bitarray, archivo_de_bloques, superbloque);
			}
		}
		else if (fcb->direct_pointer != 0 && fcb->indirect_pointer == 0 && bloques == 1)
		{
			bitarray_clean_bit(bitarray, fcb->direct_pointer);
			fcb->direct_pointer = 0;
		}
		else if (fcb->direct_pointer != 0 && fcb->indirect_pointer != 0 && bloques > 0)
		{
			bloques_restantes = liberar_bloques(archivo_de_bloques, *fcb, bitarray, nro_bloques, superbloque);
			if (bloques_restantes)
			{
				bitarray_clean_bit(bitarray, fcb->direct_pointer);
				bitarray_clean_bit(bitarray, fcb->direct_pointer);
				fcb->direct_pointer = 0;
				bloques_restantes--;
				if (bloques_restantes)
				{
					log_info(logger, "No se puede truncar el archivo.");
				}
			}
		}
	}
	guardar_fcb(archivo_fcb, *fcb);
	fclose(archivo_fcb);
	free(fcb);
}

void guardar_fcb(FILE *archivo_fcb, t_fcb fcb)
{
	t_config *fcb_config;
	config_set_value(fcb_config, "NOMBRE_ARCHIVO", fcb.name);
	config_set_value(fcb_config, "TAMANIO_ARCHIVO", fcb.size);
	config_set_value(fcb_config, "PUNTERO_DIRECTO", fcb.direct_pointer);
	config_set_value(fcb_config, "PUNTERO_INDIRECTO", fcb.indirect_pointer);
	config_save_in_file(fcb_config, dirname(archivo_fcb));
	config_destroy(fcb_config);
}

int liberar_bloques(FILE *archivo_de_bloques, t_fcb fcb, t_bitarray* bitarray, int nro_bloques, t_superbloque superbloque)
{
	while (nro_bloques != 0 && fcb.indirect_pointer != 0)
	{
		quitar_bloque_de_lista(archivo_de_bloques, bitarray, superbloque, fcb.indirect_pointer);
		nro_bloques--;
	}
	return nro_bloques;
}

int buscar_bloque_disponible(t_bitarray *bitarray, t_superbloque superbloque)
{
	int i = 0;
	bool bit = bitarray_test_bit(bitarray, i);
	while (i < superbloque.block_count && bit != 0)
	{
		i++;
	}
	return i;
}

void agregar_bloque_a_lista(FILE *archivo_de_bloques, t_bitarray *bitarray, t_superbloque superbloque, int indirect_pointer)
{
	int nuevo_bloque = buscar_bloque_disponible(bitarray, superbloque);
	uint32_t read;
	unsigned i = 0;
	bitarray_set_bit(bitarray, nuevo_bloque);
	t_bloque bloque = *leer_bloque(archivo_de_bloques, superbloque, indirect_pointer);
	while (read != 0)
	{
		i++;
	}
	bloque.data[i] = (uint32_t*)nuevo_bloque;
	escribir_bloque(archivo_de_bloques, bloque, indirect_pointer, superbloque);
}

void quitar_bloque_de_lista(FILE *archivo_de_bloques, t_bitarray *bitarray, t_superbloque superbloque, int indirect_pointer)
{
	t_bloque bloque;
	uint32_t *read;
	uint32_t eliminado = 0;
	int i = 0;
	char *ruta = dirname(archivo_de_bloques);
	fseek(archivo_de_bloques, superbloque.block_size * indirect_pointer, SEEK_SET);
	fread(&bloque, superbloque.block_size, 1, archivo_de_bloques);
	while (read != 0)
	{
		read = bloque.data[i];
		i++;
	}
	eliminado = bloque.data[i - 4];
	bitarray_clean_bit(bitarray, eliminado);
	bloque.data[i - 4] = '\0';
	fseek(archivo_de_bloques, superbloque.block_size * indirect_pointer, SEEK_SET);
	fwrite(&bloque, superbloque.block_size, 1, archivo_de_bloques);
}

void asignar_bloque(t_fcb *fcb, t_bitarray *bitarray, FILE *archivo_de_bloques, t_superbloque superbloque)
{
	if (fcb->direct_pointer == 0)
	{
		fcb->direct_pointer = buscar_bloque_disponible(bitarray, superbloque);
		bitarray_set_bit(bitarray, fcb->direct_pointer);
	}
	else if (fcb->direct_pointer != 0 && fcb->indirect_pointer == 0)
	{
		fcb->indirect_pointer = buscar_bloque_disponible(bitarray, superbloque);
		bitarray_set_bit(bitarray, fcb->indirect_pointer);
		agregar_bloque_a_lista(archivo_de_bloques, bitarray, superbloque, fcb->indirect_pointer);
	}
}

t_bitarray *leer_bitmap(FILE *bitmap, t_bitarray *bitarray, t_superbloque superbloque)
{
	bool temp_value;
	t_bitarray* leido;
	fseek(bitmap, 0, SEEK_SET);
	for (int i = 0; i < superbloque.block_count; i++)
	{
		fread(&temp_value, superbloque.block_size, 1, bitarray);
		if (temp_value)
		{
			bitarray_set_bit(bitarray, i);
		}
		else
		{
			bitarray_clean_bit(bitarray, i);
		}
	}
	return bitarray;
}

void escribir_bitmap(FILE *bitmap, t_bitarray *bitarray, t_superbloque superbloque)
{
	bool temp_value;
	fseek(bitmap, 0, SEEK_SET);
	for (int i = 0; i < superbloque.block_count; i++)
	{
		temp_value = bitarray_test_bit(bitarray, i);
		fwrite(&temp_value, superbloque.block_count, 1, bitarray);
	}
}

void escribir_bloque(FILE *archivo_de_bloques, t_bloque bloque, unsigned nro_bloque, t_superbloque superbloque)
{
	fwrite(&bloque, superbloque.block_size * nro_bloque, 1, archivo_de_bloques);
}

t_bloque* leer_bloque(FILE *archivo_de_bloques, t_superbloque superbloque, unsigned nro_bloque)
{
	t_bloque* bloque;
	fseek(archivo_de_bloques, superbloque.block_size * nro_bloque, SEEK_SET);
	fread(bloque, superbloque.block_size, 1, archivo_de_bloques);
	return bloque;
}

t_list *leer_archivo(FILE *archivo_de_bloques, char *nombre_archivo, t_superbloque superbloque)
{
	t_fcb fcb = leer_fcb(nombre_archivo);
	t_list *bloques = list_create();
	t_bloque *bloque;
	t_bloque bloque_aux;
	int32_t nro_bloque;
	fseek(archivo_de_bloques, superbloque.block_size * fcb->direct_pointer, SEEK_SET);
	fread(bloque, superbloque.block_size, 1, archivo_de_bloques);
	list_add(bloques, bloque);
	fseek(archivo_de_bloques, superbloque.block_size * fcb->indirect_pointer, SEEK_SET);
	fread(bloque, superbloque.block_size, 1, archivo_de_bloques);
	nro_bloque = bloque->data[i];
	fseek(archivo_de_bloques, superbloque.block_size * nro_bloque, SEEK_SET);
	read(&bloque_aux, superbloque.block_size, 1, archivo_de_bloques);
	list_add(bloques, &bloque_aux);
	while (nro_bloque != 0)
	{
		nro_bloque = bloque->data[i];
		fseek(archivo_de_bloques, superbloque.block_size * nro_bloque, SEEK_SET);
		read(&bloque_aux, superbloque.block_size, 1, archivo_de_bloques);
		list_add(bloques, &bloque_aux);
	}
	return bloques;
}

void escribir_archivo(FILE *archivo_de_bloques, char *nombre_archivo, uint32_t *data)
{
	t_fcb fcb = leer_fcb(nombre_archivo);
	t_list *bloques = list_create();
	t_list *leidos = list_create();
	uint32_t *direcciones, leido;
	t_bloque bloque;
	int i = 0, j = 0;
	fseek(archivo_de_bloques, superbloque.block_size * fcb->direct_pointer, SEEK_SET);
	bloque.data = data[i];
	fwrite(&bloque, superbloque.block_size * fcb->direct_pointer, 1, archivo_de_bloques);
	i++;
	int32_t nro_bloque;
	while (i < bloques->elements_count)
	{
		bloque.data = data[i];
		list_add(bloques, bloque);
		i++;
		;
	}
	fread(leido, sizeof(uint32_t) * superbloque.block_size, 1, fcb->indirect_pointer);
	while (j < superbloque.block_size / sizeof(uint32_t))
	{
		list_add(leidos, leido[j]);
	}
	while (bloques != NULL)
	{

		fwrite(&bloque, superbloque.block_size * fcb->indirect_pointer, 1, archivo_de_bloques);
	}
	while (j < leidos->elements_count)
	{
		bloque = *list_get(bloques, j);
		escribir_bloque(archivo_de_bloques, bloque, j, superbloque);
	}
}*/
