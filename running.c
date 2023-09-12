#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "running.h"
#include "queue.h"
#include "pcb.h"
#include "init.h"
#include "structs.h"

#define NUM_READY_QUEUES 3

static Pcb *highest_prio;
static Pcb *running;

int random_counts[3] = {0,0,0};

static int choose_random_queue()
{
    // All ready queues are empty == INIT running.
    if (List_count(Queue_get(0)) == 0 &&
        List_count(Queue_get(1)) == 0 &&
        List_count(Queue_get(2)) == 0)
    {
        return -1;
    }

    // Percents:        {0.50, 0.35, 0.15}
    // correspond to    {0,    1,    2}
    int prio;
    int random = rand() % 100;
    if (random < 50)
    {
        prio = 0;
    }
    else if (random < 85)
    {
        prio = 1;
    }
    else 
    {
        prio = 2;
    }
    // If the chosen list is empty, then just 50/50 between the remaining 2.
    // Wish I had a better implementation for this, but.................
    if (List_count(Queue_get(prio)) == 0)
    {
        random = rand() % 2;
        printf("prio %d\n", prio);
        printf("random %d\n", random);
        if (random == 0)
        {
            if (prio == 0)
            {
                prio = 1;
                if (List_count(Queue_get(prio)) == 0)
                {
                    prio = 2;
                }
            }
            if (prio == 1)
            {
                prio = 0;
                if (List_count(Queue_get(prio)) == 0)
                {
                    prio = 2;
                }
            }
            if (prio == 2)
            {
                prio = 0;
                if (List_count(Queue_get(prio)) == 0)
                {
                    prio = 1;
                }
            }
        }
        else if (random == 1)
        {
            if (prio == 0)
            {
                prio = 2;
                if (List_count(Queue_get(prio)) == 0)
                {
                    prio = 1;
                }
            }
            if (prio == 1)
            {
                prio = 2;
                if (List_count(Queue_get(prio)) == 0)
                {
                    prio = 0;
                }
            }
            if (prio == 2)
            {
                prio = 1;
                if (List_count(Queue_get(prio)) == 0)
                {
                    prio = 0;
                }
            }
        }
    }
    random_counts[prio] += 1;
    return prio;
}

void Running_init()
{
    running = Init_get();
    srand(time(NULL));
}

Pcb *Running_get()
{
    return running;
}

// Only use case is Pcb_create(), when INIT is running.
// Necessary b/c can't call Running_update() in all cases when creating new PCB.
// (would cause undesired pre-empting)
void Running_set(Pcb *pcb)
{
    running = pcb;
    running->status = "RUNNING";
}

void Running_update()
{       
    int prio = 0;
    while (prio < NUM_READY_QUEUES && \
    (highest_prio = List_last(Queue_get(prio))) == NULL)
    {
        prio += 1;
    }

    if (prio > NUM_READY_QUEUES)
    {
        puts("ERROR: Running_update() is somehow going past the ready queue bounds.");
        exit(1);
    }

    // No processes in ready queues
    if (highest_prio == NULL && running->id == INIT)
    {
        puts("The current process is still Process INIT.");
        return;
    }
    
    // No change in Process
    if (running == highest_prio)
    {
        printf("The current process is still Process %d.\n", running->id);
        running->status = "RUNNING";
        return;
    }

    running = highest_prio; // Running can now be NULL (Init) or a real process.

    if (running == NULL) // TODO: I think this is ok
    {
        running = Init_get();
        puts("There are no more Processes in the Ready Queues.\nThe current Process has been updated to INIT.");
    }

    else 
    {
        running->status = "RUNNING";
        printf("The current Process has been updated to Process %d.\n", running->id);
    }

    // Also, print out running's message field if it has received one
    if (running->msg.received == true)
    {   
        printf("Message received from Process %d:\n", running->msg.sender_id);
        puts("~~~~~~~~~~~~~~~~~\n");
        printf("%s\n", running->msg.text);
        puts("~~~~~~~~~~~~~~~~~");
        // Reset message.
        running->msg.text[0] = '\0';
        running->msg.received = false;
    }

}

// void Running_update_attempt2()
// {
//     running = Running_get();

//     int prio = 0;

//     while (prio < NUM_READY_QUEUES)
//     {
//         // Find a non-empty list.
//         if (List_count(Queue_get(prio)) == 0)
//         {
//             prio += 1;
//             continue;
//         }

//         float percent_time_spent = ready_queue_array[prio].time / total_time;
//         float percent_share = ready_queue_array[prio].percent_share;

//         if (percent_time_spent < percent_share)
//         {
//             break;
//         }

//         // Even if the queue has exceeded its time share, must check if the other
//         // queues are empty. If they are, then it still gets to run.
//         int i = prio + 1;
//         while (i < NUM_READY_QUEUES &&
//                List_count(Queue_get(i)) == 0)
//         {
//             i += 1;
//         }
//         if (i == NUM_READY_QUEUES)
//         {
//             // All the other lists are empty.
//             break;
//         }
//         else
//         {
//             // i holds the prio of a non-empty list. Set prio equal to it
//             // and repeat the outer while loop to check its percentages.
//             prio = i;
//             continue;
//         }
//     }

//     // If this point is reached, then "prio" could either hold our
//     // desired, non-empty ready queue, or it could be the last ready queue.
//     // We haven't checked if the last queue is empty yet, so we must do that here.
//     if (List_count(Queue_get(prio)) == 0)
//     {  
//         running = Init_get();
//     }
//     else
//     {
//         running = List_last(Queue_get(prio));
//     }
// }

void Running_update_random_version()
{       
    int priority = choose_random_queue();
    if (priority == -1 && running->id == INIT)
    {
        puts("The current Process is still Process INIT.");
    }
    else if (priority == -1 && running->id != INIT)
    {
        running = Init_get();
        puts("The current Process has been updated to Process INIT.");
    }
    else 
    {
        running = List_last(Queue_get(priority));
        running->status = "RUNNING";
        printf("The Scheduling Algorithm has chosen Process %d as the current Process.\n", running->id);
    }

    // Also, print out running's message field if it has received one
    if (running->msg.received == true)
    {   
        printf("Message received from Process %d:\n", running->msg.sender_id);
        puts("~~~~~~~~~~~~~~~~~\n");
        printf("%s\n", running->msg.text);
        puts("~~~~~~~~~~~~~~~~~");
        // Reset message.
        running->msg.text[0] = '\0';
        running->msg.received = false;
    }

    // TODO comment out: testing random avg
    printf("q0 count: %d\n", random_counts[0]);
    printf("q1 count: %d\n", random_counts[1]);
    printf("q2 count: %d\n", random_counts[2]);

}


char *Running_check()
{
    if (running->id == INIT)
    {
        return "RUNNING";
    }
    else
    {
        return "READY";
    }
}
