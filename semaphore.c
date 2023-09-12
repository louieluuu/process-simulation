#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "semaphore.h"
#include "running.h"
#include "queue.h"
#include "init.h"

#define NUM_SEMAPHORES 5

static Semaphore semaphores_array[NUM_SEMAPHORES];
static int semaphore_count = 0;

static char str_id[3];
static int int_id;
Pcb *running;

void Semaphore_init()
{
    for (int i = 0; i < NUM_SEMAPHORES; i++)
    {
        semaphores_array[i].value = 1;
        semaphores_array[i].blocked = NULL;
    }
}

void Semaphore_create()
{
    if (semaphore_count == NUM_SEMAPHORES)
    {
        puts("ERROR: All available Semaphores already created.");
        return;
    }

    while (1)
    {
        puts("Please enter a Semaphore ID from 0-4 to create.");
        fgets(str_id, sizeof(str_id), stdin);
        int_id = str_id[0] - '0'; // convert char to int

        if (int_id < 0 || int_id > NUM_SEMAPHORES-1)
        {
            puts("ERROR: Invalid input.");
        }
        else if (semaphores_array[int_id].blocked != NULL)
        {
            printf("ERROR: Semaphore %d already created.\n", int_id);
        }
        else 
        {
            break;
        }
    }
    semaphores_array[int_id].blocked = List_create();
    if (semaphores_array[int_id].blocked == NULL)
    {
        puts("ERROR: List_create() failed. Lists required to execute simulation. Exiting program.");
        exit(1);
    }
    semaphore_count += 1;
    printf("Semaphore %d created.\n", int_id);
}

void Semaphore_P()
{
    if (semaphore_count == 0)
    {
        puts("ERROR: No Semaphores exist to call P() on.");
        puts("Try creating a Semaphore with N.");
        return;
    }
    while (1)
    {
        puts("Please enter a Semaphore ID from 0-4 to call P() on.");
        fgets(str_id, sizeof(str_id), stdin);
        int_id = str_id[0] - '0'; // convert char to int

        if (int_id < 0 || int_id > NUM_SEMAPHORES-1)
        {
            puts("ERROR: Invalid input.");
        }
        else if ((semaphores_array[int_id].blocked == NULL))
        {
            printf("ERROR: Semaphore %d has not been created.\n", int_id);
        }
        else
        {
            break;
        }
    }
    semaphores_array[int_id].value -= 1;

    printf("Semaphore %d now has a value of %d.\n",\
    int_id, semaphores_array[int_id].value);

    if (semaphores_array[int_id].value < 0)
    {
        running = Running_get();
        if (running->id != INIT)
        {
            Queue_dequeue(running->ready_queue);
            Queue_enqueue(semaphores_array[int_id].blocked, running);
            running->status = "BLOCKED";
            running->ready_queue = semaphores_array[int_id].blocked;
            printf("Process %d has been blocked on Semaphore %d.\n", \
            running->id, int_id);
            Running_update();
        }        
    }
}

void Semaphore_V()
{
    if (semaphore_count == 0)
    {
        puts("ERROR: No Semaphores exist to call V() on.");
        puts("Try creating a Semaphore with N.");
        return;
    }
    puts("Calling V() on a Semaphore.");
    while (1)
    {
        puts("Please enter a Semaphore ID from 0-4 to call V() on.");
        fgets(str_id, sizeof(str_id), stdin);
        int_id = str_id[0] - '0'; // convert char to int

        if (int_id < 0 || int_id > NUM_SEMAPHORES-1)
        {
            puts("ERROR: Invalid input.");
        }
        else if ((semaphores_array[int_id].blocked == NULL))
        {
            printf("ERROR: Semaphore %d has not been created.\n", int_id);
        }
        else
        {
            break;
        }
    }
    semaphores_array[int_id].value += 1;

    printf("Semaphore %d now has a value of %d.\n", int_id, semaphores_array[int_id].value);

    if (semaphores_array[int_id].value <= 0)
    {
        if (List_count(semaphores_array[int_id].blocked) != 0)
        {
            Pcb *woken = Queue_dequeue(semaphores_array[int_id].blocked);
            woken->status = "READY";
            woken->ready_queue = Queue_get(woken->priority);
            Queue_enqueue(woken->ready_queue, woken);
            printf("Process %d has been woken up and re-queued into Ready Queue %d.\n", \
            woken->id, woken->priority);

            // Edge case: Init calling V() causes a wake up
            // (Normally, running shouldn't update after a V() call.)
            running = Running_get();
            if (running->id == INIT)
            {
                // Running_set(woken);
                Running_update();
            }
        }
    }
}

Pcb *Semaphore_search_all_blocked(int id)
{
    for (int i = 0; i < NUM_SEMAPHORES; i++)
    {
        List *blocked = semaphores_array[i].blocked;
        if (blocked == NULL)
        {
            continue;
        }
        Pcb *search_result = Queue_search(blocked, id);
        if (search_result == NULL)
        {
            continue;
        }
        else
        {
            return search_result;
        }
    }
    return NULL;
}

void Semaphore_print_all_blocked()
{
    for (int i = 0; i < NUM_SEMAPHORES; i++)
    {
        printf("Semaphore %d Blocked: ", i);
        List *blocked = semaphores_array[i].blocked;

        if (blocked == NULL)
        {
            printf("\n");
            continue;
        }
        Pcb *ptr = List_last(blocked);
        if (ptr == NULL)
        {
            printf("\n");
            continue;
        }
        while (ptr != NULL)
        {
            printf("| P%d ", ptr->id);
            ptr = List_prev(blocked);
        }
        puts("|");
    }
}

void Semaphore_shutdown()
{
    for (int i = 0; i < NUM_SEMAPHORES; i++)
    {
        if (semaphores_array[i].blocked != NULL)
        {
            List_free(semaphores_array[i].blocked, free);
        }
    }
}