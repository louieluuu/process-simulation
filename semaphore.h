#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include <stdbool.h>

#include "list.h"
#include "pcb.h"

typedef struct sem {
    int value;
    List *blocked;

} Semaphore;

void Semaphore_init();

void Semaphore_create();

void Semaphore_P();

void Semaphore_V();

Pcb *Semaphore_search_all_blocked();

void Semaphore_print_all_blocked();

void Semaphore_shutdown();

#endif