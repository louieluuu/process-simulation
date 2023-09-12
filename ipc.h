#ifndef _IPC_H_
#define _IPC_H_

#include <stdbool.h>
#include "pcb.h"

#include "structs.h"

// typedef struct Message Msg;
// struct Message {
//     char text[40];
//     int sender_id; 
//     bool received;
// };

void Ipc_init();

void Ipc_send();

void Ipc_receive();

void Ipc_reply();

Pcb *Ipc_search_all_blocked(int id);

void Ipc_print_blocked();

void Ipc_print_all_blocked();

void Ipc_shutdown();

#endif