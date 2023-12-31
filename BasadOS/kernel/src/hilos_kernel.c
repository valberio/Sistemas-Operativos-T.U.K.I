#include "hilos_kernel.h"

void crear_hilo(pthread_t hilo, int conexion_deseada, void *(*funcion_wrapper)(void *))
{
	Parametros_de_hilo parametros_hilo;
	parametros_hilo.conexion = conexion_deseada;
	pthread_create(&hilo, NULL, funcion_wrapper, (void *)&parametros_hilo);
	pthread_detach(hilo);
}

void *crear_proceso_wrapper(void *arg)
{
	Parametros_de_hilo *args = (Parametros_de_hilo *)arg;
	char *parametro = args->mensaje;
	int conexion = args->conexion;
	double estimacion = args->estimacion;
	int conexion_kernel_memoria = args->pid;
	crear_proceso(parametro, conexion, estimacion, conexion_kernel_memoria);
	return NULL;
}

void crear_proceso(char *codigo_recibido, int socket_consola, double estimado_inicial, int conexion_kernel_memoria)
{
	t_pcb *pcb = crear_pcb(codigo_recibido, socket_consola, estimado_inicial);

	t_paquete *paquete = crear_paquete();
	paquete->codigo_operacion = INICIALIZAR_PROCESO;
	paquete->buffer = serializar_contexto(pcb->contexto_de_ejecucion);

	enviar_paquete(paquete,conexion_kernel_memoria);
	eliminar_paquete(paquete);
	paquete = recibir_paquete(conexion_kernel_memoria);
	t_contexto_de_ejecucion *contexto_actualizado = malloc(sizeof(t_contexto_de_ejecucion));
	contexto_actualizado = deserializar_contexto_de_ejecucion(paquete->buffer);
	eliminar_paquete(paquete);
	pcb->contexto_de_ejecucion  = contexto_actualizado;

	
	log_info(logger, "Se crea el proceso %i en NEW", pcb->pid);
	sem_wait(&mutex_cola_new);
	queue_push(cola_new, pcb);
	sem_post(&mutex_cola_new);
	sem_post(&semaforo_de_procesos_para_ejecutar);
}

void *recibir_de_consolas_wrapper(void *arg)
{
	Parametros_de_hilo *args = (Parametros_de_hilo *)arg;
	int parametro = args->conexion;
	int conexion_kernel_memoria = args->pid;

	recibir_de_consolas(parametro,conexion_kernel_memoria);
	return NULL;
}

void recibir_de_consolas(int server_consola,int conexion_kernel_memoria)
{
	int i = 0;

	while (conexion_kernel_memoria)
	{
		int conexion_consola = esperar_cliente(server_consola);
		char *codigo_recibido = recibir_mensaje(conexion_consola);
		double estimado_de_rafaga = config_get_double_value(config, "ESTIMACION_INICIAL") / 1000;
		pthread_t hilo_creador_de_proceso;
		Parametros_de_hilo parametros_hilo_crear_proceso;
		parametros_hilo_crear_proceso.mensaje = codigo_recibido;
		parametros_hilo_crear_proceso.conexion = conexion_consola;
		parametros_hilo_crear_proceso.estimacion = estimado_de_rafaga;
		parametros_hilo_crear_proceso.pid = conexion_kernel_memoria;
		pthread_create(&hilo_creador_de_proceso, NULL, crear_proceso_wrapper, (void *)&parametros_hilo_crear_proceso);
		pthread_join(hilo_creador_de_proceso, NULL);
		i++;
	}
}

void *manipulador_de_IO_wrapper(void *arg)
{
	Parametros_de_hilo *args = (Parametros_de_hilo *)arg;
	char *tiempo = args->mensaje;
	int pid = args->conexion;
	manipulador_de_IO(tiempo, pid);
	return NULL;
}

void manipulador_de_IO(char *tiempo_en_blocked, int pid)
{
	int tiempo = atoi(tiempo_en_blocked);
	sleep(tiempo);
	t_pcb *pcb;

	bool buscar_proceso(void *elemento)
	{
		return buscar_pid(elemento, pid);
	}

	sem_wait(&mutex_cola_blocked);
	pcb = list_get(list_filter(cola_blocked->elements, buscar_proceso), 0);
	list_remove_by_condition(cola_blocked->elements, buscar_proceso);
	sem_post(&mutex_cola_blocked);

	sem_wait(&mutex_cola_ready);
	time(&(pcb->tiempo_de_llegada_a_ready));
	queue_push(cola_ready, pcb);
	sem_post(&mutex_cola_ready);
	log_info(logger, "PID: %i - Estado Anterior: BLOCKED - Estado Actual: READY", pcb->pid);
	sem_post(&semaforo_procesos_en_ready);
}
