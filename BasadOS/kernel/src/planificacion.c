#include "planificacion.h"

t_pcb *salida_FIFO()
{
	char *pid_char;
	char *lista_pids = malloc(1);
	lista_pids[0] = '\0';
	t_pcb *pcb_cola;
	sem_wait(&mutex_cola_ready);

	for (int i = 0; i < list_size(cola_ready->elements); i++)
	{
		pcb_cola = list_get(cola_ready->elements, i);
		pid_char = convertir_a_char(pcb_cola->pid);
		lista_pids = realloc(lista_pids, strlen(lista_pids) + strlen(pid_char) + 1);
		strcat(lista_pids, pid_char);
	}

	log_info(logger, "Cola Ready: FIFO : %s", lista_pids);

	t_pcb *pcb = queue_pop(cola_ready);
	sem_post(&mutex_cola_ready);
	return pcb;
}

void calcular_estimado_de_rafaga(t_pcb *pcb)
{
	double alfa = config_get_double_value(config, "HRRN_ALFA");
	pcb->tiempo_de_la_ultima_rafaga = ((double)(pcb->fin_de_uso_de_cpu - pcb->inicio_de_uso_de_cpu) / CLOCKS_PER_SEC) * 1000;
	pcb->estimado_rafaga = alfa * pcb->tiempo_de_la_ultima_rafaga + (1 - alfa) * pcb->estimado_rafaga;
}

t_pcb *salida_HRRN()
{
	char *pid_char;
	char *lista_pids = malloc(1);
	lista_pids[0] = '\0';
	t_pcb *pcb_cola;
	sem_wait(&mutex_cola_ready);
	t_pcb *pcb;
	time_t tiempo_actual;
	time(&tiempo_actual);
	bool es_el_HRR(void *un_proceso, void *otro_proceso)
	{
		return el_mayor_hrr_entre(un_proceso, otro_proceso, tiempo_actual);
	}
	sem_wait(&mutex_cola_ready);

	list_sort(cola_ready->elements, es_el_HRR);
	for (int i = 0; i < list_size(cola_ready->elements); i++)
	{
		pcb_cola = list_get(cola_ready->elements, i);
		pid_char = convertir_a_char(pcb_cola->pid);
		lista_pids = realloc(lista_pids, strlen(lista_pids) + strlen(pid_char) + 1); // +1 for the null-terminator
		strcat(lista_pids, pid_char);
	}

	log_info(logger, "Cola Ready: HRRN : %s", lista_pids);
	pcb = queue_pop(cola_ready);
	sem_post(&mutex_cola_ready);

	return pcb;
}

bool el_mayor_hrr_entre(t_pcb *un_proceso, t_pcb *otro_proceso, time_t tiempo_actual)
{
	double tiempo_esperando_en_ready_un_proceso, tiempo_esperando_en_ready_otro_proceso;
	tiempo_esperando_en_ready_un_proceso = difftime(un_proceso->tiempo_de_llegada_a_ready, tiempo_actual);
	tiempo_esperando_en_ready_otro_proceso = difftime(un_proceso->tiempo_de_llegada_a_ready, tiempo_actual);
	return (tiempo_esperando_en_ready_un_proceso + un_proceso->estimado_rafaga) / un_proceso->estimado_rafaga > (tiempo_esperando_en_ready_otro_proceso + otro_proceso->estimado_rafaga) / otro_proceso->estimado_rafaga;
}
char *convertir_a_char(uint32_t numero)
{
	char *nuevo_valor = malloc(10 * sizeof(char));
	if (nuevo_valor != NULL)
	{
		sprintf(nuevo_valor, "%u", numero);
	}
	return nuevo_valor;
}