#include "manejo_recursos.h"
void crear_lista_de_recursos(t_list *recursos, char **recursos_array, char **instancias_array)
{
	size_t cantidad_recursos = contarCadenas(recursos_array);
	Recurso *recurso[cantidad_recursos];
	for (int i = 0; i < cantidad_recursos; i++)
	{
		recurso[i] = malloc(sizeof(Recurso));
		recurso[i]->recurso = malloc(strlen(recursos_array[i]) + 1);
		strcpy(recurso[i]->recurso, recursos_array[i]);
		recurso[i]->instancias = atoi(instancias_array[i]);
		recurso[i]->cola_de_bloqueados = queue_create();
		list_add(recursos, recurso[i]);
	}
}

int wait_recurso(char *recurso, t_pcb *proceso)
{
	Recurso *recurso_solicitado;
	log_info(logger, "El recurso solicitado es %s\n", recurso);
	t_list *lista_con_recurso = list_create();
	bool obtenerRecurso(void *elemento)
	{
		Recurso *recurso_a_obtener;
		recurso_a_obtener = elemento;

		return strcmp(recurso_a_obtener->recurso, recurso) == 0;
	}
	lista_con_recurso = list_filter(recursos, obtenerRecurso);
	if (list_size(lista_con_recurso) == 0)
	{
		log_info(logger, "El recurso solicitado no existe");
		sem_wait(&mutex_cola_exit);
		queue_push(cola_exit, proceso);
		sem_post(&mutex_cola_exit);

		log_info(logger, "PID: %i - Estado Anterior: RUNNING - Estado Actual: EXIT", proceso->pid);
		log_info(logger, "Finaliza el proceso %i - Motivo: INVALID_RESOURCE", proceso->pid);

		sem_post(&semaforo_procesos_en_exit);

		return 1;
	}
	recurso_solicitado = list_get(lista_con_recurso, 0);
	recurso_solicitado->instancias -= 1;
	log_info(logger, "PID: %i - Wait: %s - Instancias: %i", proceso->pid, recurso_solicitado->recurso, recurso_solicitado->instancias);
	if (recurso_solicitado->instancias < 0)
	{
		queue_push(recurso_solicitado->cola_de_bloqueados, proceso);
		log_info(logger, "PID: %i - Bloqueado por: %s", proceso->pid, recurso_solicitado->recurso);
		return 1;
	}
	return 0;
}

int signal_recurso(char *recurso, t_pcb *proceso)
{
	Recurso *recurso_solicitado;
	t_list *lista_con_recurso = list_create();
	bool obtenerRecurso(void *elemento)
	{
		Recurso *recurso_a_obtener;
		recurso_a_obtener = elemento;
		return strcmp(recurso_a_obtener->recurso, recurso) == 0;
	}
	lista_con_recurso = list_filter(recursos, obtenerRecurso);
	if (list_size(lista_con_recurso) == 0)
	{
		log_info(logger, "El recurso mencionado no existe");
		sem_wait(&mutex_cola_exit);
		queue_push(cola_exit, proceso);
		sem_post(&mutex_cola_exit);
		log_info(logger, "PID: %i - Estado Anterior: RUNNING - Estado Actual: EXIT", proceso->pid);
		log_info(logger, "Finaliza el proceso %i - Motivo: INVALID_RESOURCE", proceso->pid);
		sem_post(&semaforo_procesos_en_exit);
		return 1;
	}
	recurso_solicitado = list_get(lista_con_recurso, 0);
	recurso_solicitado->instancias += 1;
	log_info(logger, "PID: %i - Signal: %s - Instancias: %i", proceso->pid, recurso_solicitado->recurso, recurso_solicitado->instancias);
	if (recurso_solicitado->instancias <= 0)
	{
		t_pcb *proceso_bloqueado_por_recurso;
		proceso_bloqueado_por_recurso = queue_pop(recurso_solicitado->cola_de_bloqueados);

		sem_wait(&mutex_cola_ready);
		time(&(proceso_bloqueado_por_recurso->tiempo_de_llegada_a_ready));
		queue_push(cola_ready, proceso_bloqueado_por_recurso);
		// log_info(logger,"Cola Ready %i: [%s]");
		sem_post(&mutex_cola_ready);
		log_info(logger, "PID: %i - Estado Anterior: BLOCKED - Estado Actual: READY", proceso_bloqueado_por_recurso->pid);

		sem_post(&semaforo_procesos_en_ready);
	}
	return 0;
}

t_list *procesos_bloqueados_por_recursos()
{
	t_list *procesos_bloqueados = list_create();
	int cantidad_recursos = list_size(recursos);

	for (int i = 0; i < cantidad_recursos; i++)
	{
		Recurso *recurso = list_get(recursos, i);
		int cantidad_procesos_bloqueados = list_size(recurso->cola_de_bloqueados->elements);
		for (int i = 0; i < cantidad_procesos_bloqueados; i++)
		{
			t_pcb *proceso_bloqueado = list_get(recurso->cola_de_bloqueados->elements, i);
			list_add(procesos_bloqueados, proceso_bloqueado);
		}
	}
	return procesos_bloqueados;
}

size_t contarCadenas(char **array)
{
	size_t contador = 0;

	while (array[contador] != NULL)
	{
		contador++;
	}

	return contador;
}

bool buscar_pid(void *pcb, int pid)
{
	return ((t_pcb *)pcb)->pid == pid;
}
