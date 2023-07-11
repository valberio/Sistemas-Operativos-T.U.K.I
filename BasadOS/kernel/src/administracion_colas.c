#include "administracion_colas.h"
void *administrar_procesos_de_exit_wrapper(void *arg)
{
	Parametros_de_hilo *args = (Parametros_de_hilo *)arg;
	int parametro = args->conexion;
	administrar_procesos_de_exit(parametro);
	return NULL;
}
void administrar_procesos_de_exit(int conexion_kernel_memoria)
{
	while (1)
	{
		sem_wait(&semaforo_procesos_en_exit);
		t_pcb *proceso_a_finalizar;
		t_paquete *paquete = crear_paquete();
		crear_buffer(paquete);
		paquete->codigo_operacion = 1;

		sem_wait(&mutex_cola_exit);
		proceso_a_finalizar = queue_pop(cola_exit);
		sem_post(&mutex_cola_exit);

		// ACA VA TU LOGICA DE MATARLOS

		t_paquete *paquete_a_memoria = crear_paquete();
		paquete->codigo_operacion = FINALIZAR_PROCESO;
		paquete->buffer = serializar_contexto(proceso_a_finalizar->contexto_de_ejecucion);

		enviar_paquete(paquete_a_memoria, conexion_kernel_memoria);
		eliminar_paquete(paquete_a_memoria);
		enviar_paquete(paquete, proceso_a_finalizar->socket_consola);
		eliminar_paquete(paquete);
		sem_post(&semaforo_multiprogramacion);
	}
}

void *administrar_procesos_de_new_wrapper(void *arg)
{
	Parametros_de_hilo *args = (Parametros_de_hilo *)arg;
	int parametro = args->conexion;
	administrar_procesos_de_new(parametro);
	return NULL;
}

void administrar_procesos_de_new(int cliente_cpu)
{ // TODO: mandarle los procesos en NEW a memoria para que les ponga el segm. 0
	while (cliente_cpu)
	{

		sem_wait(&semaforo_de_procesos_para_ejecutar);

		sem_wait(&semaforo_multiprogramacion);

		sem_wait(&mutex_cola_new);
		t_pcb *nuevo_pcb = queue_pop(cola_new);
		sem_post(&mutex_cola_new);

		sem_wait(&mutex_cola_ready);
		time(&(nuevo_pcb->tiempo_de_llegada_a_ready)); // ACA EMPIEZA A CORRER SU TIEMPO EN READY
		queue_push(cola_ready, nuevo_pcb);
		sem_post(&mutex_cola_ready);

		log_info(logger, "PID: %i - Estado anterior: NEW - Estado actual: READY", nuevo_pcb->pid);

		sem_post(&semaforo_procesos_en_ready);
	}
}

void administrar_procesos_de_ready(int cliente_cpu, int cliente_memoria, int cliente_filesystem)
{
	while (cliente_cpu)
	{
		// ESPERA A QUE HAYA POR LO MENOS 1 PROCESO EN READY
		sem_wait(&semaforo_procesos_en_ready);

		t_pcb *proceso_en_ejecucion;
		char *planificador = config_get_string_value(config, "ALGORITMO_PLANIFICACION");

		if (strcmp(planificador, "HRRN") == 0)
		{
			proceso_en_ejecucion = salida_HRRN();
		}

		if (strcmp(planificador, "FIFO") == 0)
		{
			proceso_en_ejecucion = salida_FIFO();
		}

		log_info(logger, "PID: %i - Estado anterior: READY - Estado actual: RUNNING", proceso_en_ejecucion->pid);
		proceso_en_ejecucion->inicio_de_uso_de_cpu = clock(); // ACA SE INICIALIZA EL TIEMPO EN EJECUCION

		// PLANIFICACION
		log_info(logger, "EL ESTIMADO DE RAFAGA %f\n", proceso_en_ejecucion->estimado_rafaga);
		enviar_contexto_de_ejecucion(proceso_en_ejecucion->contexto_de_ejecucion, cliente_cpu);

		int ejecucion = 1;

		while (ejecucion)
		{
			t_paquete *paquete = recibir_contexto_de_ejecucion(cliente_cpu);

			t_contexto_de_ejecucion *contexto_actualizado = malloc(sizeof(t_contexto_de_ejecucion));
			contexto_actualizado = deserializar_contexto_de_ejecucion(paquete->buffer);
			proceso_en_ejecucion->contexto_de_ejecucion = contexto_actualizado;
			char *parametros_retorno;
			t_paquete *paquete_respuesta;

			switch (paquete->codigo_operacion)
			{

			case INTERRUPCION_A_READY: // Caso YIELD
				// Actualizo el Proceso_en_ejecucion y lo mando a ready
				sem_wait(&mutex_cola_ready);
				proceso_en_ejecucion->fin_de_uso_de_cpu = clock();
				calcular_estimado_de_rafaga(proceso_en_ejecucion);
				queue_push(cola_ready, proceso_en_ejecucion);
				sem_post(&mutex_cola_ready);
				sem_post(&semaforo_procesos_en_ready);
				log_info(logger, "PID: %i - Estado anterior: RUNNING - Estado actual: READY", proceso_en_ejecucion->pid);
				ejecucion = 0;
				break;

			case FINALIZACION: // Caso EXIT

				sem_wait(&mutex_cola_exit);
				queue_push(cola_exit, proceso_en_ejecucion);
				sem_post(&mutex_cola_exit);

				sem_post(&semaforo_procesos_en_exit);
				log_info(logger, "PID: %i - Estado anterior: RUNNING - Estado actual: EXIT", proceso_en_ejecucion->pid);
				log_info(logger, "Finaliza el proceso %i - Motivo: SUCCES", proceso_en_ejecucion->pid);
				ejecucion = 0;
				// Actualizo el Proceso_en_ejecucion y lo mando a exit
				// Mando un mensaje a la consola del proceso avisándole que completó la ejecución
				// Mando un paquete con buffer vacio y código de operación EXIT

				break;

			case INTERRUPCION_BLOQUEANTE: // Caso I/O, tengo que recibir el tiempo que se bloquea el proceso
				parametros_retorno = recibir_mensaje(cliente_cpu);
				pthread_t hilo_procesos_IO[50];
				int i = 0;

				Parametros_de_hilo parametros_IO;
				parametros_IO.mensaje = parametros_retorno;
				parametros_IO.conexion = proceso_en_ejecucion->pid;

				sem_wait(&mutex_cola_blocked);
				proceso_en_ejecucion->fin_de_uso_de_cpu = clock();
				calcular_estimado_de_rafaga(proceso_en_ejecucion);
				queue_push(cola_blocked, proceso_en_ejecucion);
				sem_post(&mutex_cola_blocked);

				pthread_create(&hilo_procesos_IO[i], NULL, manipulador_de_IO_wrapper, (void *)&parametros_IO);
				pthread_detach(hilo_procesos_IO[i]);

				log_info(logger, "PID: %i - Ejecuta IO: %s", proceso_en_ejecucion->pid, parametros_retorno);
				log_info(logger, "PID: %i - Estado anterior: RUNNING - Estado actual: BLOCKED", proceso_en_ejecucion->pid);
				log_info(logger, "PID: %i - Bloqueado por: IO", proceso_en_ejecucion->pid);
				i++;
				ejecucion = 0;
				break;

			case PETICION_RECURSO: // Caso WAIT, proceso pide un recurso
				parametros_retorno = recibir_mensaje(cliente_cpu);
				if (wait_recurso(parametros_retorno, proceso_en_ejecucion) == 0)
				{
					enviar_mensaje("0", cliente_cpu);
				}
				else
				{
					enviar_mensaje("1", cliente_cpu);
					ejecucion = 0;
				}
				break;

			case LIBERACION_RECURSO: // Caso SIGNAL, proceso libera un recurso
				parametros_retorno = recibir_mensaje(cliente_cpu);
				if (signal_recurso(parametros_retorno, proceso_en_ejecucion) == 0)
				{
					enviar_mensaje("0", cliente_cpu);
				}
				else
				{
					enviar_mensaje("1", cliente_cpu);
					ejecucion = 0;
				}
				break;

			case CREAR_SEGMENTO:
				// CPU me pide que le pida a memoria que cree un segmento

				t_paquete *paquete_a_memoria = crear_paquete();
				paquete_a_memoria->codigo_operacion = CREAR_SEGMENTO;
				paquete_a_memoria->buffer = serializar_contexto(contexto_actualizado);

				// Recibo parámetros de CPU
				char *id = recibir_mensaje(cliente_cpu);
				log_info(logger, "Recibi de CPU %s", id);
				char *tamanio = recibir_mensaje(cliente_cpu);
				log_info(logger, "Recibi de CPU %s", tamanio);

				// Envio contexto
				enviar_paquete(paquete_a_memoria, cliente_memoria);
				eliminar_paquete(paquete_a_memoria);
				// Envio parametros
				enviar_mensaje(id, cliente_memoria);
				free(id);
				enviar_mensaje(tamanio, cliente_memoria);
				free(tamanio);
				log_info(logger, "Envie parametros");

				// Espero el OK de memoria
				// TODO: cambiar a un paquete
				paquete_respuesta = recibir_paquete(cliente_memoria);

				log_info(logger, "Recibi la respuesta de memoria");
				log_info(logger, "El PID %i ahora tiene %i segmentos", proceso_en_ejecucion->contexto_de_ejecucion->pid, list_size(proceso_en_ejecucion->contexto_de_ejecucion->tabla_segmentos));
				switch (paquete_respuesta->codigo_operacion)
				{
				case SEGMENTO_CREADO:
					t_contexto_de_ejecucion *contexto_respuesta = deserializar_contexto_de_ejecucion(paquete_respuesta->buffer);
					eliminar_paquete(paquete_respuesta);
					proceso_en_ejecucion->contexto_de_ejecucion = contexto_respuesta;
					sem_wait(&mutex_cola_ready);
					queue_push(cola_ready, proceso_en_ejecucion);
					sem_post(&mutex_cola_ready);
					sem_post(&semaforo_procesos_en_ready);
					break;
				case COMPACTACION_NECESARIA:
					// checkear operaciones entre filesystem y memoria

					enviar_mensaje("compactar", cliente_memoria);

					paquete_respuesta = recibir_paquete(cliente_memoria);
					t_list *segmentos_actualizados = deserializar_lista_de_segmentos(paquete_respuesta->buffer);
					eliminar_paquete(paquete_respuesta);
					actualizar_tablas_de_segmentos(segmentos_actualizados, contexto_actualizado->tabla_segmentos);
					paquete_a_memoria->buffer = serializar_contexto(contexto_actualizado);

					enviar_paquete(paquete_a_memoria, cliente_memoria);
					// Envio parametros
					enviar_mensaje(id, cliente_memoria);
					enviar_mensaje(tamanio, cliente_memoria);

					paquete_respuesta = recibir_paquete(cliente_memoria);
					contexto_respuesta = deserializar_contexto_de_ejecucion(paquete_respuesta->buffer);
					eliminar_paquete(paquete_respuesta);
					proceso_en_ejecucion->contexto_de_ejecucion = contexto_respuesta;

					sem_wait(&mutex_cola_ready);
					queue_push(cola_ready, proceso_en_ejecucion);
					sem_post(&mutex_cola_ready);
					sem_post(&semaforo_procesos_en_ready);
					break;
				case OUT_OF_MEMORY:
					eliminar_paquete(paquete_respuesta);
					sem_wait(&mutex_cola_exit);
					queue_push(cola_exit, proceso_en_ejecucion);
					sem_post(&mutex_cola_exit);

					sem_post(&semaforo_procesos_en_exit);
					log_info(logger, "PID: %i - Estado anterior: RUNNING - Estado actual: EXIT", proceso_en_ejecucion->pid);
					log_info(logger, "Finaliza el proceso %i - Motivo: OUT_OF_MEMORY", proceso_en_ejecucion->pid);
					break;
				default:
					break;
				}

				ejecucion = 0;

				break;

			case ELIMINAR_SEGMENTO:
				// CPU me pide que le pida a memoria que elimine un segmento
				t_paquete *paquete_a_memoria1 = crear_paquete();
				paquete_a_memoria1->codigo_operacion = ELIMINAR_SEGMENTO;
				paquete_a_memoria1->buffer = serializar_contexto(contexto_actualizado);
				char *id_a_eliminar = recibir_mensaje(cliente_cpu);
				log_info(logger, "Recibi de CPU %s", id_a_eliminar);

				enviar_paquete(paquete_a_memoria1, cliente_memoria);
				eliminar_paquete(paquete_a_memoria1);
				enviar_mensaje(id_a_eliminar, cliente_memoria);

				paquete_respuesta = recibir_paquete(cliente_memoria);
				t_contexto_de_ejecucion *contexto_respuesta = deserializar_contexto_de_ejecucion(paquete_respuesta->buffer);
				proceso_en_ejecucion->contexto_de_ejecucion = contexto_respuesta;
				ejecucion = 0;

				sem_wait(&mutex_cola_ready);
				queue_push(cola_ready, proceso_en_ejecucion);
				sem_post(&mutex_cola_ready);
				sem_post(&semaforo_procesos_en_ready);

				break;

			case ABRIR_ARCHIVO:
				// ENVIO EL TIPO DE OPERACION AL FS
				parametros_retorno = recibir_mensaje(cliente_cpu);
				int buscar_archivo = buscar_archivo_en_tabla_global(parametros_retorno);
				// ACA CREO LA ESTRUCTURA DEL ARCHIVO PARA LA TABLA DEL PROCESO
				Archivo_de_proceso *archivo = crear_archivo_para_tabla_proceso(parametros_retorno);
				list_add(proceso_en_ejecucion->tabla_archivos_abiertos, archivo);
				log_info(logger, "PID: %i - Abrir Archivo: %s", proceso_en_ejecucion->pid, parametros_retorno);
				if (buscar_archivo == 0)
				{ // caso de que el archivo no estuviese abierto
					// Creo la estructura del archivo PARA LA TABLA GLOBAL
					Archivo *nuevo_archivo = crear_archivo(parametros_retorno);
					list_add(lista_archivos_abiertos, nuevo_archivo);
					// Y la añado a la lista de archivos abiertos global
					t_paquete *paquete = crear_paquete();
					paquete->codigo_operacion = ABRIR_ARCHIVO;
					enviar_paquete(paquete, cliente_filesystem);
					// ENVIO EL NOMBRE DEL ARCHIVO REQUERIDO
					enviar_mensaje(parametros_retorno, cliente_filesystem);
					enviar_mensaje("0", cliente_cpu);
				}
				else
				{
					aniadir_a_bloqueados(proceso_en_ejecucion, parametros_retorno);
					ejecucion = 0;
					enviar_mensaje("Se bloqueo el proceso", cliente_cpu);
				}
				break;
			case CERRAR_ARCHIVO:
				parametros_retorno = recibir_mensaje(cliente_cpu);

				gestionar_cierre_archivo(parametros_retorno);
				borrar_de_tabla_de_archivos(proceso_en_ejecucion, parametros_retorno);
				// LOGICA CON FILESYSTEM INTRODUCIR AQUI
				log_info(logger, "PID: %i - Cerrar Archivo: %s", proceso_en_ejecucion->pid, parametros_retorno);
				break;
			case ACTUALIZAR_PUNTERO:
				parametros_retorno = recibir_mensaje(cliente_cpu);
				uint32_t valor_puntero = atoi(recibir_mensaje(cliente_cpu));
				actualizar_puntero(proceso_en_ejecucion, parametros_retorno, valor_puntero);
				break;
			case TRUNCAR_ARCHIVO:
				parametros_retorno = recibir_mensaje(cliente_cpu);
				char *nuevo_tamano = recibir_mensaje(cliente_cpu);
				Parametros_de_hilo parametros_hilo_kernel_filesystem;
				parametros_hilo_kernel_filesystem.conexion = cliente_filesystem;
				parametros_hilo_kernel_filesystem.mensaje = parametros_retorno;
				parametros_hilo_kernel_filesystem.valor = nuevo_tamano;
				parametros_hilo_kernel_filesystem.pid = proceso_en_ejecucion->pid;
				sem_wait(&mutex_cola_blocked);
				queue_push(cola_blocked, proceso_en_ejecucion);
				sem_post(&mutex_cola_blocked);
				pthread_t hilo_truncador_de_archivos;
				pthread_create(&hilo_truncador_de_archivos, NULL, solicitar_truncamiento, (void *)&parametros_hilo_kernel_filesystem);
				pthread_detach(hilo_truncador_de_archivos);
				ejecucion = 0;
				break;
			default:
				break;
			}
			eliminar_paquete(paquete);

		}
	}
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
	// ENVIO EL NOMBRE DEL ARCHIVO REQUERIDO
	enviar_mensaje(nombre_archivo, cliente_filesystem);
	enviar_mensaje(nuevo_tamano, cliente_filesystem);
	char *respuesta = recibir_mensaje(cliente_filesystem);

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
	queue_push(cola_ready, proceso_a_recuperar);
	sem_post(&mutex_cola_ready);
	sem_post(&semaforo_procesos_en_ready);

	return NULL;
}

Archivo *crear_archivo(char *nombre_archivo)
{
	Archivo *archivo = malloc(sizeof(Archivo));
	archivo->nombre_archivo = malloc(sizeof(nombre_archivo));
	strcpy(archivo->nombre_archivo, nombre_archivo);
	archivo->procesos_bloqueados = queue_create();
	return archivo;
}

Archivo_de_proceso *crear_archivo_para_tabla_proceso(char *nombre_archivo)
{
	Archivo_de_proceso *archivo = malloc(sizeof(Archivo_de_proceso));
	archivo->nombre = malloc(sizeof(nombre_archivo));
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
		Archivo *archivo_en_tabla;
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
		Archivo *archivo_en_tabla;
		archivo_en_tabla = elemento;

		return strcmp(archivo_en_tabla->nombre_archivo, nombre_archivo) == 0;
	}
	Archivo *archivo = list_find(lista_archivos_abiertos, existe_el_archivo);
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
		Archivo_de_proceso *archivo_en_tabla;
		archivo_en_tabla = elemento;

		return strcmp(archivo_en_tabla->nombre, nombre_archivo) == 0;
	}
	Archivo_de_proceso *archivo = list_find(proceso->tabla_archivos_abiertos, existe_el_archivo);
	list_remove_by_condition(proceso->tabla_archivos_abiertos, existe_el_archivo);
	free(archivo->nombre);
	free(archivo);
}

void actualizar_puntero(t_pcb *proceso, char *nombre_archivo, uint32_t valor_puntero)
{
	bool existe_el_archivo(void *elemento)
	{
		Archivo_de_proceso *archivo_en_tabla;
		archivo_en_tabla = elemento;

		return strcmp(archivo_en_tabla->nombre, nombre_archivo) == 0;
	}
	Archivo_de_proceso *archivo = list_find(proceso->tabla_archivos_abiertos, existe_el_archivo);
	archivo->puntero = valor_puntero;
	log_info(logger, "PID: %i - Actualizar puntero Archivo: %s - Puntero %i", proceso->pid, archivo->nombre, archivo->puntero);
}

void actualizar_tablas_de_segmentos(t_list *segmentos_actualizados, t_list *segmentos_en_running)
{
	while (list_size(segmentos_actualizados) > 0)
	{
		Segmento *segmento_actualizado = list_get(segmentos_actualizados, 0);
		bool reemplazar_si_coinciden_ids(void *un_elemento)
		{
			Segmento *un_segmento = un_elemento;
			return un_segmento->id == segmento_actualizado->id;
		}

		sem_wait(&mutex_cola_ready);
		list_replace_by_condition(cola_ready->elements, reemplazar_si_coinciden_ids, segmento_actualizado);
		sem_post(&mutex_cola_ready);

		sem_wait(&mutex_cola_exit);
		list_replace_by_condition(cola_exit->elements, reemplazar_si_coinciden_ids, segmento_actualizado);
		sem_post(&mutex_cola_exit);

		sem_wait(&mutex_cola_blocked);
		list_replace_by_condition(cola_blocked->elements, reemplazar_si_coinciden_ids, segmento_actualizado);
		sem_post(&mutex_cola_blocked);

		list_replace_by_condition(segmentos_en_running, reemplazar_si_coinciden_ids, segmento_actualizado);
		list_remove(segmentos_actualizados, 0);
		for (int i = 0; i < list_size(segmentos_en_running); i++)
		{
			Segmento *sas = list_get(segmentos_en_running, i);
			log_info(logger, "SEGMENTO ID: %d, DESPLAZAMIENTO: %d", sas->id, sas->desplazamiento);
		}
	}
}