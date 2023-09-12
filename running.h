#ifndef _RUNNING_H_
#define _RUNNING_H_

#include "pcb.h"
#include "structs.h"

void Running_init();

Pcb *Running_get();

void Running_set(Pcb *pcb);

void Running_update_old();

void Running_update();

char *Running_check();

// TODO: need a Running_shutdown() or no? (not malloced, but check the memory)

#endif