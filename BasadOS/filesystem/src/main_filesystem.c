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

int abrir_archivo(char* nombre_archivo, t_superbloque* superbloque, t_bitarray* bitarray){
	char* ruta;
	strcpy(ruta, "../files/");
	strcat(ruta, nombre_archivo);
	strcat(ruta, ".config");
	FILE* archivo = fopen(ruta,"rb");
	if(archivo != NULL){
		fclose(archivo);
		return 0;
	}
	else{
		log_info(logger, "No existe el archivo al que se intenta acceder.");
	}
}

int crear_archivo(char* nombre_archivo, t_superbloque* superbloque, t_bitarray* bitarray){
	char* ruta;
	strcpy(ruta, "../files/");
	strcat(ruta, nombre_archivo);
	strcat(ruta, ".config");
	FILE* archivo = fopen(ruta,"wb");
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
	strcat(ruta, ".config");
	t_config* fcb = config_create(ruta);
	config_set_value(fcb, "NOMBRE_ARCHIVO", nombre_archivo);
	config_set_value(fcb, "TAMANIO_ARCHIVO", "0");
	config_set_value(fcb, "PUNTERO_DIRECTO", "0");
	config_set_value(fcb, "PUNTERO_INDIRECTO", "0");
	config_save(fcb);
	free(ruta);
	return 0;
}

t_fcb leer_fcb(char* nombre_archivo){
	t_fcb fcb;
	char* ruta;
	strcpy(ruta, "../files/");
	strcat(ruta, nombre_archivo);
	strcat(ruta, ".config");
	t_config* fcb_config = config_create(ruta);
	fcb.direct_pointer = config_get_int_value(fcb_config, "PUNTERO_DIRECTO");
	fcb.indirect_pointer = config_get_int_value(fcb_config, "PUNTERO_INDIRECTO");
	fcb.size = config_get_int_value(fcb_config, "TAMANIO_ARCHIVO");
	strcpy(fcb.name, config_get_int_value(fcb_config, "NOMBRE_ARCHIVO"));
	return fcb;
}

int truncar_archivo(char* nombre_archivo, int nro_bloques, t_superbloque superbloque, t_bitarray* bitarray, int bloques, t_log* logger){
	/*Al momento de truncar un archivo, pueden ocurrir 2 situaciones: 
Ampliar el tamaño del archivo: Al momento de ampliar el tamaño del archivo deberá actualizar el tamaño del archivo en el FCB y se le deberán asignar tantos bloques como sea necesario para poder direccionar el nuevo tamaño.
Reducir el tamaño del archivo: Se deberá asignar el nuevo tamaño del archivo en el FCB y se deberán marcar como libres todos los bloques que ya no sean necesarios para direccionar el tamaño del archivo (descartando desde el final del archivo hacia el principio).
*/
	char* ruta;
	t_fcb fcb;
	unsigned bloques_restantes;
	strcpy(ruta, "../files/");
	strcat(ruta, nombre_archivo);
	strcat(ruta, ".config");
	FILE* archivo_fcb
	if(abrir_archivo(nombre_archivo, superbloque, bitarray)){
		fcb = leer_fcb(nombre_archivo);
		if(fcb.direct_pointer == 0 && bloques <= 0){
			log_info(logger, "No se puede truncar el archivo.");
		}
		else if (bloques >= 0){ //Ampliar el tamaño del archivo
			for(int i = 0; i < bloques; i++){
				asignar_bloque(fcb, bitarray, archivo_de_bloques);
			}
		}
		else if(fcb.direct_pointer != 0 && fcb.indirect_pointer == 0 && bloques == 1){
				bitarray_clean_bit(bitarray, direct_pointer);
				fcb.direct_pointer = 0;
		}
		else if(fcb.direct_pointer != 0 && fcb.indirect_pointer != 0 && bloques > 0){
			bloques_restantes = liberar_bloques(archivo_de_bloques, fcb, bitarray, nro_bloques, superbloque);
			if(bloques_restantes){
				bitarray_clean_bit(bitarray, direct_pointer);
				bitarray_clean_bit(bitarray, fcb.direct_pointer);
				fcb.direct_pointer = 0;
				bloques_restantes--;
				if(bloques_restantes){
					log_info(logger, "No se puede truncar el archivo.");
				}
			}
		}
	}
	guardar_fcb(archivo_fcb, fcb);
	fclose(archivo_fcb);
}

void guardar_fcb(FILE* archivo_fcb, t_fcb fcb){
	t_config* fcb_config;
	config_set_value(fcb_config, "NOMBRE_ARCHIVO", fcb.name);
	config_set_value(fcb_config, "TAMANIO_ARCHIVO", fcb.size);
	config_set_value(fcb_config, "PUNTERO_DIRECTO", fcb.direct_pointer);
	config_set_value(fcb_config, "PUNTERO_INDIRECTO", fcb.indirect_pointer);
	config_save_in_file(fcb_config, dirname(archivo_fcb));
	config_destroy(fcb_config);
}

int liberar_bloques(FILE* archivo_de_bloques, t_fcb fcb, t_bitarray bitarray, int nro_bloques, t_superbloque superbloque){
	while(nro_bloques != 0 && fcb.indirect_pointer != 0){
		quitar_bloque_de_lista(archivo_de_bloques, bitarray, superbloque, fcb.indirect_pointer);
	}
	return int;
}

int buscar_bloque_disponible(t_bitarray* bitarray, t_superbloque superbloque){
	int i = 0;
	bool bit = bitarray_test_bit(bitarray, i);
	while(i < superbloque.block_count && bit != 0){
		i++;
	}
	return i;
}

void agregar_bloque_a_lista(FILE* archivo_de_bloques, t_bitarray* bitarray, t_superbloque superbloque, int indirect_pointer){
	int nuevo_bloque = buscar_bloque_disponible(bitarray, superbloque);
	bitarray_set_bit(bitarray, nuevo_bloque);
	t_bloque bloque = leer_bloque(archivo_de_bloques, superbloque, indirect_pointer);
	strcat(bloque.data, (char*) nuevo_bloque);
	escribir_bloque(archivo_de_bloques, indirect_pointer, superbloque);
}

void quitar_bloque_de_lista(FILE* archivo_de_bloques, t_bitarray* bitarray, t_superbloque superbloque, int indirect_pointer){
	t_bloque bloque;
	char read;
	unint31_t eliminado = 0;
	int i = 0;
	char* ruta = dirname(archivo_de_bloques);
	fseek(archivo_de_bloques, superbloque.block_size * indirect_pointer,SEEK_SET);
	fread(bloque, superbloque.block_size, 1, archivo_de_bloques);
	while(read != '\0'){
		read = bloque.data[i];
		i++
	}
	eliminado = bloque.data[i-4];
	bitarray_clean_bit(bitarray, eliminado);
	bloque.data[i-4] = '\0';
	fseek(archivo_de_bloques, superbloque.block_size * indirect_pointer,SEEK_SET);
	frwite(bloque, superbloque.block_size, 1, archivo_de_bloques);
}


void asignar_bloque(t_fcb* fcb, t_bitarray* bitarray, FILE* archivo_de_bloques){
	if(fcb->direct_pointer == 0){
		fcb->direct_pointer = buscar_bloque_disponible(bitarray, superbloque);
		bitarray_set_bit(bitarray, fcb->direct_pointer);
	}
	else if (fcb->direct_pointer != 0 && fcb->indirect_pointer == 0){
		fcb->indirect_pointer = buscar_bloque_disponible(bitarray, superbloque);
		bitarray_set_bit(bitarray, fcb->indirect_pointer);
		agregar_bloque_a_lista()
	}
}

t_bitarray* leer_bitmap(FILE* bitmap t_bitarray* bitarray, t_superbloque t_superbloque){
	bool temp_value;
	fseek(bitmap, 0, seek_set);
	for(int i = 0; i < t_superbloque.block_count; i++){
		fread(temp_value, size, 1, *bitarray);
		if(temp_value){
			bitarray_set_bit(bitarray, i);
		}
		else{
			bitarray_clean_bit(bitarray, i);
		}
	}
	return bitarray;
}

void escribir_bitmap(FILE* bitmap t_bitarray* bitarray, t_superbloque t_superbloque){
	bool temp_value;
	fseek(bitmap, 0, seek_set);
	for(int i = 0; i < t_superbloque.block_count; i++){
		temp_value = bitarray_test_bit(bitarray, i);
		fwrite(temp_value, size, 1, *bitarray);
	}
}

void escribir_bloque(FILE* archivo_de_bloques, t_bloque bloque, unsigned bloque, t_superbloque superbloque){
	fwrite(bloque, superbloque.block_size, 1, archivo_de_bloques);
}

t_bloque leer_bloque(FILE* archivo_de_bloques, t_superbloque superbloque, unsigned nro_bloque){
	t_bloque bloque;
	fseek(archivo_superbloque, superbloque.block_size * nro_bloque, seek_set);
	fread(bloque, superbloque.block_size, 1, archivo_de_bloques);
	return bloque;
}