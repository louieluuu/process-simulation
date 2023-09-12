#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "init.h"
#include "pcb.h"
#include "queue.h"
#include "running.h"

static Pcb *init;

void Init_init() 
{
    init = malloc(sizeof(Pcb));
    init->id = INIT;
    init->status = "INEVITABLE";
    init->priority = -1;
    init->ready_queue = NULL;
    init->msg.text[0] = '\0';
    init->msg.received = false;
}

Pcb *Init_get()
{
    return init;
}

/*
void Init_set_msg()
{
    // Disallow multiple messages
    if (init->msg.text[0] != '\0')
    {
        puts("ERROR: Process INIT's message field is full.\n");
        return;
    }

    puts("Please enter the message (max 40 chars) to send to Process INIT.");
    char msg[40];
    fgets(msg, sizeof(msg), stdin);
    // TODO: error checking, msg > 40 char

    strcpy(result_search->msg.text, msg); // TODO: strncpy?

    Pcb *running = Running_get();

    init->msg.sender_id = running->id;
    if (running->id != INIT)
    {

    }

    puts("Message successfully sent.");
}
*/

void Init_shutdown()
{
    free(init);
}