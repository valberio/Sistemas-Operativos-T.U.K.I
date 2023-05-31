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
} // esto esta bien

t_pcb* salida_HRRN(){
	t_pcb* pcb;
	//t_pcb* pcb_hrr;
	time_t tiempo_actual;
	//double tiempo_esperando_en_ready, highest_response_ratio;
	//int indice;
	//highest_response_ratio = 0;
	time(&tiempo_actual);
	bool es_el_HRR(void* un_proceso,void* otro_proceso){
		return el_mayor_hrr_entre(un_proceso,otro_proceso,tiempo_actual);
	}
	sem_wait(&mutex_cola_ready);
	list_sort(cola_ready->elements,es_el_HRR);
	pcb = queue_pop(cola_ready);
	sem_post(&mutex_cola_ready);

	return pcb;
	// for(int i = 0; i< queue_size(cola_ready); i ++){
	// 	pcb = list_get(cola_ready->elements, i);
	// 	tiempo_esperando_en_ready = difftime(pcb->tiempo_de_llegada_a_ready,tiempo);
	// 	if((tiempo_esperando_en_ready + pcb->estimado_rafaga) / pcb->estimado_rafaga > highest_response_ratio){
	// 		highest_response_ratio = (tiempo_esperando_en_ready + pcb->estimado_rafaga) / pcb->estimado_rafaga;
	// 		pcb_hrr = list_get(cola_ready->elements, i);
	// 		indice = i;
	// 	}
	
	// list_sort(cola_ready->elements, (tiempo_esperando_en_ready + pcb->estimado_rafaga) / pcb->estimado_rafaga > highest_response_ratio) DEBERIA FUNCIONAR
	// queue_pop(cola_ready) DEBERIA FUNCIONAR
	//list_remove(cola_ready->elements,indice);
	//return pcb_hrr;
}
bool el_mayor_hrr_entre(t_pcb* un_proceso, t_pcb* otro_proceso, time_t tiempo_actual){
	double tiempo_esperando_en_ready_un_proceso, tiempo_esperando_en_ready_otro_proceso;
	tiempo_esperando_en_ready_un_proceso = difftime(un_proceso->tiempo_de_llegada_a_ready,tiempo_actual);
	tiempo_esperando_en_ready_otro_proceso = difftime(un_proceso->tiempo_de_llegada_a_ready,tiempo_actual);
	return (tiempo_esperando_en_ready_un_proceso + un_proceso->estimado_rafaga) / un_proceso->estimado_rafaga > (tiempo_esperando_en_ready_otro_proceso + otro_proceso->estimado_rafaga) / otro_proceso->estimado_rafaga;

}

void manipulador_de_IO(char* tiempo_en_blocked, int pid){
	int tiempo = atoi(tiempo_en_blocked);
	printf("EMPIEZO A DORMIR\n");
	sleep(tiempo);
	t_pcb* pcb;

	bool buscar_proceso(void* elemento){
		return buscar_pid(elemento, pid);
	}
	
	sem_wait(&mutex_cola_blocked);	
	pcb = list_get(list_filter(cola_blocked->elements, buscar_proceso),0);
	list_remove_by_condition(cola_blocked->elements, buscar_proceso);	
	sem_post(&mutex_cola_blocked);

	sem_wait(&mutex_cola_ready);
	queue_push(cola_ready, pcb);
	sem_post(&mutex_cola_ready);

	sem_post(&semaforo_procesos_en_ready);
}

bool buscar_pid(void* pcb, int pid){
	return ((t_pcb* )pcb)->pid == pid;
}

