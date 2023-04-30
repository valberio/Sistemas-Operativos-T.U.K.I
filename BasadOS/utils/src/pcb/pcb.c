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

void liberar_pcb(t_pcb* pcb)
{
	// free(pcb->contexto_de_ejecucion.registros);
	list_destroy_and_destroy_elements(pcb->contexto_de_ejecucion.lista_instrucciones, free);
	list_destroy_and_destroy_elements(pcb->tabla_segmentos, free);
	list_destroy_and_destroy_elements(pcb->tabla_archivos_abiertos, free);
	liberar_registros(pcb->contexto_de_ejecucion.registros);
	free(pcb->tabla_segmentos);
	free(pcb->tabla_archivos_abiertos);
	free(pcb);


	//list_destroy_and_destroy_elements(pcb->contexto_de_ejecucion, free);
}

void liberar_registros(t_registros registros){
	free(registros.AX);
	free(registros.BX);
	free(registros.CX);
	free(registros.DX);

	free(registros.EAX);
	free(registros.EBX);
	free(registros.ECX);
	free(registros.EDX);

	free(registros.RAX);
	free(registros.RBX);
	free(registros.RCX);
	free(registros.RDX);
}