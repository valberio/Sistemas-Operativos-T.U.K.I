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
	//cb->contexto_de_ejecucion.lista_instrucciones = agregar_instrucciones_a_pcb(instrucciones);
	list_add_all(pcb->contexto_de_ejecucion.lista_instrucciones, agregar_instrucciones_a_pcb(instrucciones));
	pcb->estimado_rafaga = 1;
	
	return pcb;
}

t_list* agregar_instrucciones_a_pcb(char* str)
{
	t_list* temp_list = list_create();
	char* token = strtok(str, "\n");
	while (token != NULL) {
        //printf("%s\n", token);
		list_add(temp_list, token);
        token = strtok(NULL, "\n");
    }
	return temp_list;
}