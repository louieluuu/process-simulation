#ifndef _PCB_H_
#define _PCB_H_

#include "list.h"
#include "ipc.h"

#include "structs.h"

// typedef struct pcb {
//     int id; 
//     char *status;
//     int priority;
//     List *ready_queue;
//     // char *msg;
//     Msg msg;
// } Pcb;

void Pcb_create();

void Pcb_fork();

void Pcb_kill();

void Pcb_exit();

void Pcb_quantum();

int Pcb_count();

void Pcb_count_inc();

void Pcb_count_dec();

void Pcb_print(Pcb *pcb);

#endif