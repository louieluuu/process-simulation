#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "pcb.h"
#include "list.h"

void Queue_init();

List *Queue_get(int priority);

List *Queue_get_blocked(char *choice);

void Queue_enqueue(List *queue, Pcb *pcb);

Pcb *Queue_dequeue(List *queue);

Pcb *Queue_remove(List *queue, int id);

Pcb *Queue_search(List *queue, int id);

Pcb *Queue_search_all(int id);

void Queue_print_all();

void Queue_shutdown();

#endif