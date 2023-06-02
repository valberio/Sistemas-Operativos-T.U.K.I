#include "planificacion.h"

t_pcb* salida_FIFO(){
	sem_wait(&mutex_cola_ready);
	t_pcb* pcb = queue_pop(cola_ready);
	sem_post(&mutex_cola_ready);
	return pcb;
}

void calcular_estimado_de_rafaga(t_pcb* pcb){
	double alfa = config_get_double_value(config, "HRRN_ALFA");
	pcb->tiempo_de_la_ultima_rafaga = ((double) (pcb->fin_de_uso_de_cpu - pcb->inicio_de_uso_de_cpu) / CLOCKS_PER_SEC )* 1000;
	pcb->estimado_rafaga = alfa * pcb->tiempo_de_la_ultima_rafaga + (1-alfa) * pcb->estimado_rafaga;
} 

t_pcb* salida_HRRN(){
	t_pcb* pcb;
	time_t tiempo_actual;
	time(&tiempo_actual);
	bool es_el_HRR(void* un_proceso,void* otro_proceso){
		return el_mayor_hrr_entre(un_proceso,otro_proceso,tiempo_actual);
	}
	sem_wait(&mutex_cola_ready);
	list_sort(cola_ready->elements,es_el_HRR);
	pcb = queue_pop(cola_ready);
	sem_post(&mutex_cola_ready);

	return pcb;
}

bool el_mayor_hrr_entre(t_pcb* un_proceso, t_pcb* otro_proceso, time_t tiempo_actual){
	double tiempo_esperando_en_ready_un_proceso, tiempo_esperando_en_ready_otro_proceso;
	tiempo_esperando_en_ready_un_proceso = difftime(un_proceso->tiempo_de_llegada_a_ready,tiempo_actual);
	tiempo_esperando_en_ready_otro_proceso = difftime(un_proceso->tiempo_de_llegada_a_ready,tiempo_actual);
	return (tiempo_esperando_en_ready_un_proceso + un_proceso->estimado_rafaga) / un_proceso->estimado_rafaga > (tiempo_esperando_en_ready_otro_proceso + otro_proceso->estimado_rafaga) / otro_proceso->estimado_rafaga;
}

