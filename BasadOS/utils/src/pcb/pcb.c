#include"pcb.h"
t_pcb *pcb_create(){
    t_pcb *pcb = malloc(sizeof(t_pcb));
    return pcb;
}