#include "main_filesystem.h"

/*------------------------------------------------------------------*/
/*						FINAL										*/
/*------------------------------------------------------------------*/
/*		-Funcion que devuelva el proximo bloque libre
		-Funcion que lea los punteros guardados en un bloque

																	*/
/*------------------------------------------------------------------*/


t_list* fcb_list;
t_config *config;
t_log* logger;
t_bitarray* bitarray;
uint32_t cantidad_bloques;
uint32_t tamanio_bloque;
double retardo;
char* ruta_superbloque;
char* ruta_bitmap;
char* ruta_archivo_bloques;


int main()
{
	logger = iniciar_logger("log_filesystem.log", "LOG_FILESYSTEM");
	config = iniciar_config("configs/filesystem.config");
	fcb_list = list_create();
	// Conecto filesystem como cliente a memoria
	char *puerto_a_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
	char *ip = config_get_string_value(config, "IP_MEMORIA");
	int cliente_filesystem_a_memoria = crear_conexion_al_server(logger, ip, puerto_a_memoria);

	//Superbloque
	t_config *superbloque_config = iniciar_config(config_get_string_value(config, "PATH_SUPERBLOQUE"));
	
	tamanio_bloque = config_get_double_value(superbloque_config, "BLOCK_SIZE");
	cantidad_bloques = config_get_double_value(superbloque_config, "BLOCK_COUNT");

	retardo = config_get_int_value(config, "RETARDO_ACCESO_BLOQUE") / 1000;

	//tenemos tantos bits como bloques
	//Bitmap
	ruta_bitmap = config_get_string_value(config, "PATH_BITMAP");


	bitarray = crear_bitmap(ruta_bitmap, cantidad_bloques);
	
	ruta_archivo_bloques = config_get_string_value(config, "PATH_BLOQUES");
	//vaciar_archivo_bloques(cantidad_bloques, tamanio_bloque);

	//Recorro el directorio de FCBs y creo estructuras
	recorrer_directorio_fcb(ruta_bitmap);
	escribir_archivo("Notas1erParcialK9999", "Nacho mogolico de mierda", 0, 24);
	char* datos = leer_archivo("Notas1erParcialK9999", 0, 24);
	log_info(logger, "Datos leidos %s", datos);
	free(datos);

	//truncar_archivo("Notas1erParcialK9999", 145);
	// Conecto el filesystem como servidor del kernel
	char* puerto_a_kernel = config_get_string_value(config, "PUERTO_ESCUCHA");
	int servidor_filesystem = iniciar_servidor(logger, puerto_a_kernel);
	int conexion_filesystem_kernel = esperar_cliente(servidor_filesystem);
	if (conexion_filesystem_kernel)
	{
	log_info(logger, "Filesystem recibió la conexión del kernel!");
	}
	
	//recibir_ordenes_kernel(conexion_filesystem_kernel,cliente_filesystem_a_memoria);

	/*if (cliente_filesystem_a_memoria)
	{
		log_info(logger, "Filesystem se conectó a memoria!");
		t_contexto_de_ejecucion *contexto = malloc(sizeof(t_contexto_de_ejecucion));
		contexto = recibir_contexto_de_ejecucion(cliente_filesystem_a_memoria);
		printf("Recibi instruccion %s, %s\n", list_get(contexto->instrucciones, 0), list_get(contexto->instrucciones, 1));
	}*/
	bitarray_destroy(bitarray);
}



void recibir_ordenes_kernel(int conexion_filesystem_kernel, int cliente_filesystem_a_memoria){
	while(conexion_filesystem_kernel){
		t_paquete* operacion = recibir_contexto_de_ejecucion(conexion_filesystem_kernel);
		char* nombre_archivo;
		char* puntero;
		char* cantidad_bytes;
		char* direccion_fisica;
		int cantidad_bytes_int;
		int puntero_int;
    	switch(operacion->codigo_operacion){
			case ABRIR_ARCHIVO:
				abrir_o_crear_archivo(nombre_archivo, conexion_filesystem_kernel);
				break;
			case TRUNCAR_ARCHIVO:
				char* nuevo_tamano = recibir_mensaje(conexion_filesystem_kernel);
				int tamanio = atoi(nuevo_tamano);
				truncar_archivo(nombre_archivo, tamanio);
				enviar_mensaje("OK", conexion_filesystem_kernel);

			case PETICION_LECTURA:
				puntero = recibir_mensaje(conexion_filesystem_kernel);
				cantidad_bytes = recibir_mensaje(conexion_filesystem_kernel);
				direccion_fisica = recibir_mensaje(conexion_filesystem_kernel);
				nombre_archivo = recibir_mensaje(conexion_filesystem_kernel);
				cantidad_bytes_int = atoi(cantidad_bytes);
				puntero_int = atoi(puntero);

				char* datos_de_archivo = leer_archivo(nombre_archivo, puntero_int, cantidad_bytes_int);
				
				enviar_mensaje(direccion_fisica, cliente_filesystem_a_memoria);
				enviar_mensaje(datos_de_archivo, cliente_filesystem_a_memoria);

				recibir_mensaje(cliente_filesystem_a_memoria);
			case PETICION_ESCRITURA:
				puntero = recibir_mensaje(conexion_filesystem_kernel);
				cantidad_bytes = recibir_mensaje(conexion_filesystem_kernel);
				direccion_fisica = recibir_mensaje(conexion_filesystem_kernel);
				nombre_archivo = recibir_mensaje(conexion_filesystem_kernel);
				cantidad_bytes_int = atoi(cantidad_bytes);
				puntero_int = atoi(puntero);

				enviar_mensaje(direccion_fisica, cliente_filesystem_a_memoria);
				enviar_mensaje(cantidad_bytes, cliente_filesystem_a_memoria);

				char *datos = recibir_mensaje(conexion_filesystem_kernel);

				escribir_archivo(nombre_archivo, datos,puntero_int, cantidad_bytes_int);

			default:
				break;
		}
	}
}

void recorrer_directorio_fcb() 
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
			crear_estructura_fcb(ruta_fcb);
			free(ruta_fcb);
		}
		closedir(dir);	
	} else {
        // Error al abrir el directorio
        printf("No se pudo abrir el directorio: %s\n", directorio_fcb);
	}
}


void crear_estructura_fcb(char* ruta) //habria que llamarlo crear fcb
{
	t_fcb* fcb = malloc(sizeof(t_fcb));
	t_config *fcb_config = config_create(ruta);
	fcb->direct_pointer = config_get_int_value(fcb_config, "PUNTERO_DIRECTO");
	fcb->indirect_pointer = config_get_int_value(fcb_config, "PUNTERO_INDIRECTO");
	fcb->size = config_get_int_value(fcb_config, "TAMANIO_ARCHIVO");
	fcb->name = config_get_string_value(fcb_config, "NOMBRE_ARCHIVO");
	list_add(fcb_list, fcb);
	if (fcb->indirect_pointer == -1) {
		return;
	}
	//Actualizo los bloques que usa el fcb en el bitmap
	setear_bit(fcb->direct_pointer);
	setear_bit(fcb->indirect_pointer);
	FILE* archivo_bloques = fopen("fs/bloques.dat", "rb+");
	if (archivo_bloques == NULL) {
        printf("No se pudo abrir el archivo\n");
    }
	uint32_t bytes_a_guardar = fcb->size - tamanio_bloque;
	int digitos_bloque = obtener_digitos_cant_bloque();

	if (bytes_a_guardar > 0){
		uint32_t bloques_necesarios = division_redondeada_hacia_arriba(bytes_a_guardar, tamanio_bloque);
		//log_info(logger, "Los bloques necesarios son %i", bloques_necesarios);
		log_info(logger, "Acceso Bloque - Archivo: %s - Bloque Archivo: 1 - Bloque File System %i", fcb->name, fcb->indirect_pointer);
        sleep(retardo);
		for (int i = 0; i < bloques_necesarios; i++)
		{
			char* index = obtener_puntero_bloque_libre(cantidad_bloques);
			int bloque_index = atoi(index);
			setear_bit(bloque_index);
			fseek(archivo_bloques, ((fcb->indirect_pointer * tamanio_bloque) + (i * digitos_bloque)) * sizeof(char), SEEK_SET);
    		fwrite(index, sizeof(char), digitos_bloque, archivo_bloques);
		}
	}
	fclose(archivo_bloques);

}


int obtener_digitos_cant_bloque()
{
	int digitos = 0;
	uint32_t temp = cantidad_bloques;

	while (temp /= 10)
	{
		digitos++;
	}
	return digitos + 1;
}

void abrir_o_crear_archivo(char *nombre_archivo, int conexion_filesystem_kernel)
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
		crear_archivo_fcb(nombre_archivo);
		log_info(logger, "Crear Archivo: %s", nombre_archivo);
		log_info(logger, "Abrir Archivo: %s", nombre_archivo);

	}
}

void *crear_archivo_fcb(char *nombre_archivo)
{
	char* ruta = malloc(sizeof(nombre_archivo) + 11);
	strcpy(ruta, "fs/fcb/");
	strcat(ruta, nombre_archivo);
	strcat(ruta, ".dat");
	FILE *archivo = fopen(ruta, "wb");
	if (archivo == NULL)
	{
		log_info(logger, "Error al crear el archivo.");
	}
	t_config* fcb = iniciar_config(ruta);
	config_set_value(fcb, "NOMBRE_ARCHIVO", nombre_archivo);
	config_set_value(fcb, "TAMANIO_ARCHIVO", "0");
	config_set_value(fcb, "PUNTERO_DIRECTO", "-1");
	config_set_value(fcb, "PUNTERO_INDIRECTO", "-1");
	config_save(fcb);
	crear_estructura_fcb(ruta);
	free(ruta);
	return NULL;
}

