#include"pcb.h"

int contador = 0;

t_pcb * crear_pcb( char* instrucciones)
{
    t_pcb *pcb = malloc(sizeof(t_pcb));
	pcb->estado = 1;
	pcb->contexto_de_ejecucion.program_counter = 0;
	pcb->pid = contador;
	pcb->tabla_archivos_abiertos = list_create();
	pcb->tabla_segmentos = list_create();
	pcb->contexto_de_ejecucion.lista_instrucciones = list_create();

	contador++;
	list_add(pcb->contexto_de_ejecucion.lista_instrucciones, instrucciones);
	pcb->estimado_rafaga = 1;
	
	return pcb;
}