#include "gestion_archivos.h"

void *solicitar_escritura(void *arg)
{
	Parametros_de_hilo *args = (Parametros_de_hilo *)arg;
	int cliente_filesystem = args->conexion;
	char *nombre_archivo = args->mensaje;
	char *cantidad_bytes = args->valor;
	char *direccion_fisica = args->direccion_fisica;
	char *puntero = args->puntero;
	int pid = args->pid;

	enviar_mensaje(puntero, cliente_filesystem);
	enviar_mensaje(cantidad_bytes, cliente_filesystem);
	enviar_mensaje(direccion_fisica, cliente_filesystem);
	enviar_mensaje(nombre_archivo, cliente_filesystem);
	char *respuesta = recibir_mensaje(cliente_filesystem);
	if (strcmp(respuesta, "OK") != 0)
	{
		log_info(logger, "Fallo al escribir el Archivo %s", nombre_archivo);
	}
	bool existe_el_archivo(void *elemento)
	{
		t_pcb *proceso;
		proceso = elemento;

		return proceso->pid == pid;
	}

	t_pcb *proceso_a_recuperar = malloc(sizeof(t_pcb *));
	sem_wait(&mutex_cola_blocked);
	proceso_a_recuperar = list_find(cola_blocked->elements, existe_el_archivo);
	list_remove_by_condition(cola_blocked->elements, existe_el_archivo);
	sem_post(&mutex_cola_blocked);

	sem_wait(&mutex_cola_ready);
	time(&(proceso_a_recuperar->tiempo_de_llegada_a_ready));
	queue_push(cola_ready, proceso_a_recuperar);
	sem_post(&mutex_cola_ready);

	log_info(logger, "PID: %i - Estado Anterior: BLOCKED - Estado Actual: READY", proceso_a_recuperar->pid);
	sem_post(&semaforo_procesos_en_ready);
	sem_post(&semaforo_peticiones_filesystem);
	sem_post(&semaforo_para_compactacion);
	return NULL;
}

void *solicitar_lectura(void *arg)
{
	Parametros_de_hilo *args = (Parametros_de_hilo *)arg;
	int cliente_filesystem = args->conexion;
	char *nombre_archivo = args->mensaje;
	char *cantidad_bytes = args->valor;
	char *direccion_fisica = args->direccion_fisica;
	char *puntero = args->puntero;
	int pid = args->pid;
	enviar_mensaje(puntero, cliente_filesystem);
	enviar_mensaje(cantidad_bytes, cliente_filesystem);
	enviar_mensaje(direccion_fisica, cliente_filesystem);
	enviar_mensaje(nombre_archivo, cliente_filesystem);
	char *respuesta = recibir_mensaje(cliente_filesystem);
	if (strcmp(respuesta, "OK") != 0)
	{
		log_info(logger, "Fallo al leer el Archivo %s", nombre_archivo);
	}
	bool existe_el_archivo(void *elemento)
	{
		t_pcb *proceso;
		proceso = elemento;

		return proceso->pid == pid;
	}

	t_pcb *proceso_a_recuperar = malloc(sizeof(t_pcb *));
	sem_wait(&mutex_cola_blocked);
	proceso_a_recuperar = list_find(cola_blocked->elements, existe_el_archivo);
	list_remove_by_condition(cola_blocked->elements, existe_el_archivo);
	sem_post(&mutex_cola_blocked);

	sem_wait(&mutex_cola_ready);
	time(&(proceso_a_recuperar->tiempo_de_llegada_a_ready));
	queue_push(cola_ready, proceso_a_recuperar);
	sem_post(&mutex_cola_ready);
	sem_post(&semaforo_procesos_en_ready);
	log_info(logger, "PID: %i - Estado Anterior: BLOCKED - Estado Actual: READY", proceso_a_recuperar->pid);

	sem_post(&semaforo_peticiones_filesystem);
	sem_post(&semaforo_para_compactacion);
	return NULL;
}

void *solicitar_truncamiento(void *arg)
{
	Parametros_de_hilo *args = (Parametros_de_hilo *)arg;
	int cliente_filesystem = args->conexion;
	char *nombre_archivo = args->mensaje;
	char *nuevo_tamano = args->valor;
	int pid = args->pid;
	t_paquete *paquete = crear_paquete();
	paquete->codigo_operacion = TRUNCAR_ARCHIVO;
	enviar_paquete(paquete, cliente_filesystem);
	eliminar_paquete(paquete);
	// ENVIO EL NOMBRE DEL ARCHIVO REQUERIDO
	enviar_mensaje(nombre_archivo, cliente_filesystem);
	enviar_mensaje(nuevo_tamano, cliente_filesystem);
	char *respuesta = recibir_mensaje(cliente_filesystem);
	if (strcmp(respuesta, "OK") != 0)
	{
		log_info(logger, "Fallo al truncar el Archivo %s", nombre_archivo);
	}
	bool existe_el_archivo(void *elemento)
	{
		t_pcb *proceso;
		proceso = elemento;

		return proceso->pid == pid;
	}

	t_pcb *proceso_a_recuperar = malloc(sizeof(t_pcb *));
	sem_wait(&mutex_cola_blocked);
	proceso_a_recuperar = list_find(cola_blocked->elements, existe_el_archivo);
	list_remove_by_condition(cola_blocked->elements, existe_el_archivo);
	sem_post(&mutex_cola_blocked);

	sem_wait(&mutex_cola_ready);
	time(&(proceso_a_recuperar->tiempo_de_llegada_a_ready));
	queue_push(cola_ready, proceso_a_recuperar);
	sem_post(&mutex_cola_ready);
	sem_post(&semaforo_procesos_en_ready);
	sem_post(&semaforo_peticiones_filesystem);
	log_info(logger, "PID: %i - Estado Anterior: BLOCKED - Estado Actual: READY", proceso_a_recuperar->pid);

	return NULL;
}

Archivo *crear_archivo(char *nombre_archivo)
{
	Archivo *archivo = malloc(sizeof(Archivo));
	archivo->nombre_archivo = malloc(strlen(nombre_archivo) + 1);
	strcpy(archivo->nombre_archivo, nombre_archivo);
	archivo->procesos_bloqueados = queue_create();
	return archivo;
}

Archivo_de_proceso *crear_archivo_para_tabla_proceso(char *nombre_archivo)
{
	Archivo_de_proceso *archivo = malloc(sizeof(Archivo_de_proceso));
	archivo->nombre = malloc(strlen(nombre_archivo) + 1);
	strcpy(archivo->nombre, nombre_archivo);
	archivo->puntero = 0;
	return archivo;
}

int buscar_archivo_en_tabla_global(char *nombre_archivo)
{
	t_list *lista_con_archivo = list_create();
	bool existe_el_archivo(void *elemento)
	{
		Archivo *archivo_en_tabla;
		archivo_en_tabla = elemento;

		return strcmp(archivo_en_tabla->nombre_archivo, nombre_archivo) == 0;
	}
	lista_con_archivo = list_filter(lista_archivos_abiertos, existe_el_archivo);
	if (list_size(lista_con_archivo) == 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void aniadir_a_bloqueados(t_pcb *proceso, char *nombre_archivo)
{
	bool existe_el_archivo(void *elemento)
	{
		Archivo *archivo_en_tabla = malloc(sizeof(Archivo));
		archivo_en_tabla = elemento;

		return strcmp(archivo_en_tabla->nombre_archivo, nombre_archivo) == 0;
	}
	Archivo *archivo = list_find(lista_archivos_abiertos, existe_el_archivo);
	queue_push(archivo->procesos_bloqueados, proceso);
	log_info(logger, "PID: %i - Estado anterior: RUNNING - Estado actual: BLOCKED", proceso->pid);
	log_info(logger, "PID: %i - Bloqueado por: %s", proceso->pid, nombre_archivo);
}

void gestionar_cierre_archivo(char *nombre_archivo)
{
	bool existe_el_archivo(void *elemento)
	{
		// Archivo *archivo_en_tabla = malloc(sizeof(Archivo));
		// archivo_en_tabla = elemento;
		Archivo *archivo_en_tabla = (Archivo *)elemento;
		return strcmp(archivo_en_tabla->nombre_archivo, nombre_archivo) == 0;
		// free(archivo_en_tabla);
	}
	Archivo *archivo = malloc(sizeof(Archivo));
	archivo->nombre_archivo = malloc(strlen(nombre_archivo) + 1);
	archivo = list_find(lista_archivos_abiertos, existe_el_archivo);
	if (queue_size(archivo->procesos_bloqueados) == 0)
	{
		list_remove_by_condition(lista_archivos_abiertos, existe_el_archivo);
		free(archivo->nombre_archivo);
		free(archivo);
	}
	else
	{
		t_pcb *proceso_bloqueado = queue_pop(archivo->procesos_bloqueados);
		sem_wait(&mutex_cola_ready);
		time(&(proceso_bloqueado->tiempo_de_llegada_a_ready));
		queue_push(cola_ready, proceso_bloqueado);
		sem_post(&mutex_cola_ready);
		sem_post(&semaforo_procesos_en_ready);
		log_info(logger, "PID: %i - Estado anterior: BLOCKED - Estado actual: READY", proceso_bloqueado->pid);
	}
}

void borrar_de_tabla_de_archivos(t_pcb *proceso, char *nombre_archivo)
{
	bool existe_el_archivo(void *elemento)
	{
		Archivo_de_proceso *archivo_en_tabla = malloc(sizeof(Archivo_de_proceso));
		archivo_en_tabla = elemento;

		return strcmp(archivo_en_tabla->nombre, nombre_archivo) == 0;
	}
	Archivo_de_proceso *archivo = malloc(sizeof(Archivo_de_proceso));
	archivo->nombre = malloc(strlen(nombre_archivo) + 1);
	archivo = list_find(proceso->tabla_archivos_abiertos, existe_el_archivo);
	list_remove_by_condition(proceso->tabla_archivos_abiertos, existe_el_archivo);
	free(archivo->nombre);
	free(archivo);
}

void actualizar_puntero(t_pcb *proceso, char *nombre_archivo, uint32_t valor_puntero)
{
	bool existe_el_archivo(void *elemento)
	{
		Archivo_de_proceso *archivo_en_tabla = malloc(sizeof(Archivo_de_proceso));
		archivo_en_tabla = elemento;

		return strcmp(archivo_en_tabla->nombre, nombre_archivo) == 0;
	}
	Archivo_de_proceso *archivo = malloc(sizeof(Archivo_de_proceso));
	archivo->nombre = malloc((strlen(nombre_archivo) + 1) * sizeof(char));
	archivo = list_find(proceso->tabla_archivos_abiertos, existe_el_archivo);
	if (archivo == NULL)
	{
		log_info(logger, "El archivo no existe");
		return;
	}
	archivo->puntero = valor_puntero;
}

uint32_t buscar_puntero_de_archivo(t_pcb *proceso, char *nombre_archivo)
{
	bool existe_el_archivo(void *elemento)
	{
		Archivo_de_proceso *archivo_en_tabla = malloc(sizeof(Archivo_de_proceso));
		archivo_en_tabla = elemento;

		return strcmp(archivo_en_tabla->nombre, nombre_archivo) == 0;
	}
	Archivo_de_proceso *archivo = list_find(proceso->tabla_archivos_abiertos, existe_el_archivo);
	return archivo->puntero;
}


t_list *procesos_bloqueas_por_archivos()
{
	int cantidad_de_archivos = list_size(lista_archivos_abiertos);
	t_list *procesos_bloqueados = list_create();

	for (int i = 0; i < cantidad_de_archivos; i++)
	{
		Archivo *archivo = list_get(lista_archivos_abiertos, i);
		int cantidad_procesos_bloqueados = list_size(archivo->procesos_bloqueados->elements);
		for (int i = 0; i < cantidad_procesos_bloqueados; i++)
		{
			t_pcb *proceso_bloqueado = list_get(archivo->procesos_bloqueados->elements, i);
			list_add(procesos_bloqueados, proceso_bloqueado);
		}
	}
	return procesos_bloqueados;
}
