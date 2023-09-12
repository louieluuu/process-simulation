#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "pcb.h"
#include "queue.h"
#include "running.h"
#include "ipc.h"
#include "semaphore.h"
#include "init.h"

static char str_priority[6];
static int int_priority;
static char str_id[6];
static int int_id;

static char *result_check;

// count is an accurate count of current processes in the simulation,
// while lifetime_count is a rolling total (used for naming process IDs).
static int pcb_count = 0;
static int pcb_lifetime_count = 0;

void Pcb_create()
{
    while (1)
    {
        puts("Please enter the priority of the Process to be created: ");
        puts("0 (high) | 1 (medium) | 2 (low)");
        fgets(str_priority, sizeof(str_priority), stdin);

        int_priority = str_priority[0] - '0'; // convert char to int
        if (int_priority < 0 || int_priority > 2)
        {
            puts("ERROR: Invalid input.");
        }
        else
        {
            break;
        }
    }

    // These PCB fields must be attached to a variable
    // before being placed in the PCB
    result_check = Running_check();
    List *ready_queue = Queue_get(int_priority);

    Pcb *pcb = malloc(sizeof(Pcb));
    pcb->id = pcb_lifetime_count;
    pcb->status = result_check;
    pcb->priority = int_priority;
    pcb->ready_queue = ready_queue;
    pcb->msg.text[0] = '\0';
    pcb->msg.received = false;

    Queue_enqueue(ready_queue, pcb);
    Pcb_count_inc();
    printf("Process %d has been created and placed in Ready Queue %d.\n",\
    pcb->id, pcb->priority);

    Pcb *running = Running_get();
    if (running->id == INIT)
    {
        Running_update();
    }
}

void Pcb_fork()
{
    Pcb *running = Running_get();
    if (running->id == INIT)
    {
        puts("ERROR: Fork failed on Process INIT. Fork only works on normal Processes.");
        return;
    }

    Pcb *pcb = malloc(sizeof(Pcb));
    pcb->id = pcb_lifetime_count;
    pcb->status = "READY";
    pcb->priority = running->priority;
    pcb->ready_queue = running->ready_queue; 
    pcb->msg.text[0] = '\0';
    pcb->msg.received = false;

    Queue_enqueue(pcb->ready_queue, pcb);
    Pcb_count_inc();
    printf("Process %d has been created and placed in Ready Queue %d after forking Process %d.\n",\
    pcb->id, pcb->priority, running->id);
}

void Pcb_kill()
{  
    puts("Please enter the ID of the Process to be killed.");
    fgets(str_id, sizeof(str_id), stdin);

    // This snippet makes it possible to kill Init 
    // (whose ID is arbitrarily set to -1).
    if (strncmp(str_id, "-1", 2) == 0) 
    {
        if (Pcb_count() != 0)
        {
            puts("ERROR: Kill failed on Process INIT. Some Processes still remain in the simulation.");
            return;
        }
        else if (Pcb_count() == 0)
        {
            puts("Kill called on Process INIT. Ending simulation...\n");
            Init_shutdown();
            Queue_shutdown();
            Ipc_shutdown();
            Semaphore_shutdown();
            exit(0);
        }
    }

    // Checking valid input
    for (int i = 0; i < sizeof(str_id); i++)
    {
        if (isdigit(str_id[i]) == 0)
        {
            if (str_id[i] == '\n' || str_id[i] == '\0') 
            {
                continue;
            }
            puts("ERROR: Invalid input. Must be a number.");
            return;
        }
    }
    sscanf(str_id, "%d", &int_id);

    Pcb *result_search = Queue_search_all(int_id);
    if (result_search == NULL)
    {
        printf("ERROR: Process %d not found.\n", int_id);
    }
    else
    {
        Pcb *removed = Queue_remove(result_search->ready_queue, result_search->id);
        printf("Process %d has been removed from the simulation.\n", removed->id);
        if (Running_get() == removed)
        {
            Running_update();
        }
        // Won't get a chance to free this later.
        free(removed);
        Pcb_count_dec();
    }
    
}

void Pcb_exit()
{
    Pcb *running = Running_get();
    if (Pcb_count() == 0)
    {
        puts("Exit called on Process INIT. Ending simulation...\n");
        Init_shutdown();
        Queue_shutdown();
        Ipc_shutdown();
        Semaphore_shutdown();
        exit(0);
    }
    else if (running->id == INIT)
    {
        puts("Exit failed on Process INIT. Some Processes still remain in the simulation.");
    }
    else
    {
        printf("Exiting current Process. Process %d has been removed from the simulation.\n", running->id);
        Pcb *removed = Queue_dequeue(running->ready_queue);
        free(removed);
        Pcb_count_dec();
        Running_update();
    }
}

void Pcb_quantum()
{
    Pcb *running = Running_get();

    if (running->id == INIT)
    {
        puts("Quantum called, but Process INIT is still running.");
    }

    else 
    {
        Pcb *dequeued = Queue_dequeue(running->ready_queue);
        dequeued->status = "READY";
        Queue_enqueue(dequeued->ready_queue, dequeued);
        printf("Process %d's quantum expired. Re-queued into Ready Queue %d.\n",\
        dequeued->id, dequeued->priority);
        Running_update();
    }

    
}

int Pcb_count()
{
    return pcb_count;
}

void Pcb_count_inc()
{
    pcb_count += 1;
    pcb_lifetime_count += 1;
}

void Pcb_count_dec()
{
    pcb_count -= 1;
}

void Pcb_print(Pcb *pcb)
{
    (pcb->id == INIT) ? printf("ID:         INIT\n") 
                      : printf("ID:         %d\n", pcb->id);
    printf("Status:     %s\n", pcb->status);
    printf("Priority:   %d\n", pcb->priority);
    printf("Queue:      %p\n", pcb->ready_queue);
    printf("Message:    %s\n", pcb->msg.text);
}