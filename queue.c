#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "list.h"
#include "pcb.h"
#include "ipc.h"
#include "semaphore.h"
#include "structs.h"

// Array of index-matching queues 
// (i.e. queue_priority_0 == queue_array[0]) for convenient access
List *queue_array[3];

List *queue_priority_0;
List *queue_priority_1;
List *queue_priority_2;

// Comparator function used for search()
static bool compare_ids(void *pItem, void *pComparisonArg)
{
    // Typecast and dereference as required.
    if (((Pcb *)pItem)->id == *(int *)pComparisonArg)
    {
        return 1;
    }
    return 0;
}

void Queue_init()
{
    queue_priority_0 = List_create();
    queue_priority_1 = List_create();
    queue_priority_2 = List_create();

    if (queue_priority_0 == NULL ||
        queue_priority_1 == NULL ||
        queue_priority_2 == NULL)
    {
        puts("ERROR: List_create() failed. Lists required to execute simulation. Exiting program.");
        exit(1);
    }

    queue_array[0] = queue_priority_0;
    queue_array[1] = queue_priority_1;
    queue_array[2] = queue_priority_2;
}

List *Queue_get(int priority)
{
    return queue_array[priority];
}

void Queue_enqueue(List *queue, Pcb *pcb)
{
    if (pcb->id == INIT)
    {
        return;
    }
    if (List_prepend(queue, pcb) == -1)
    {
        puts("ERROR: List full. Try killing a Process with K.");
        return;
    }
    pcb->ready_queue = queue; // TODO: should remove the 
                            // extraneous pcb->ready_queue updates...
}

Pcb *Queue_dequeue(List *queue)
{
    return List_trim(queue);
}

Pcb *Queue_search(List *queue, int id)
{
    // Make sure to set the curr ptr to first before searching,
    // since List_search leaves the ptr alone post-search.
    List_first(queue);
    return List_search(queue, compare_ids, &id);
}

Pcb *Queue_remove(List *queue, int id)
{
    // Only situations Queue_remove() is called are Pcb_kill() and Ipc_reply().
    // Should always be called on an ID that has already been checked to exist.
    if (Queue_search(queue, id) == NULL)
    {
        puts("FATAL ERROR: Queue_remove() failed. ID does not exist.");
        exit(1);
    }
    return List_remove(queue);
}

// Searches ALL queues - (1) ready queues, (2) IPC queues, (3) semaphore queues
Pcb *Queue_search_all(int id)
{
    // (1) Ready Queues 0-2 
    Pcb *search_result = NULL;

    int i = 0;
    while (i < 3 && search_result == NULL)
    {
        search_result = Queue_search(queue_array[i], id);
        i += 1;
    }
    if (i > 3)
    {
        puts("ERROR: Index i out of bounds in Queue_search_all().");
        exit(1);
    }

    if (search_result != NULL) // result found
    {
        return search_result;
    }

    // (2) Ipc queues (send & receive blocked)
    search_result = Ipc_search_all_blocked(id);
    if (search_result != NULL)
    {
        return search_result;
    }

    // (3) Semaphore blocked queues
    search_result = Semaphore_search_all_blocked(id);
    return search_result; // can be NULL now
}

void Queue_print_all()
{
    for (int i = 0; i < 3; i++)
    {
        printf("Ready Queue %d: ", i);
        if (List_count(queue_array[i]) == 0)
        {
            printf("\n");
            continue;
        }
        Pcb *ptr = List_last(queue_array[i]);
        while (ptr != NULL)
        {
            printf("| P%d ", ptr->id);
            ptr = List_prev(queue_array[i]);
        }
        puts("|");
    }
}

void Queue_shutdown()
{
    List_free(queue_priority_0, free);
    List_free(queue_priority_1, free);
    List_free(queue_priority_2, free);
}