#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "ipc.h"
#include "queue.h"
#include "running.h"
#include "pcb.h"
#include "init.h"

// Blocked queues
List *send_blocked;
List *receive_blocked;

static char str_id[6];
static int int_id;
static char msg[40];
// static char *source_txt;
// static char *complete_msg;
static Pcb *search_result;
static Pcb *running;

void Ipc_init()
{
    send_blocked = List_create();
    receive_blocked = List_create();
    if (send_blocked == NULL || receive_blocked == NULL)
    {
        puts("ERROR: List_create() failed. Lists required to execute simulation. Exiting program.");
        exit(1);
    }
}

void Ipc_send()
{
    puts("Please enter the ID of the Process to send a message to.");
    fgets(str_id, sizeof(str_id), stdin);

    // // handling this separately b/c isdigit zz
    // if (strncmp(str_id, "-1", 2) == 0) 
    // {
    //     Init_set_msg();
    //     return;
    // }

    for (int i = 0; i < sizeof(str_id); i++)
    {
        if (isdigit(str_id[i]) == 0)
        {
            if (str_id[i] == '-' || str_id[i] == '\n' || str_id[i] == '\0') 
            {
                continue;
            }
            puts("ERROR: Invalid input. Must be a number.");
            return;
        }
    }
    sscanf(str_id, "%d", &int_id);

    Pcb *result_search;

    result_search = Queue_search_all(int_id);
    if (result_search == NULL)
    {
        if (int_id == -1)
        {
            result_search = Init_get();
        }
        else 
        {
            printf("ERROR: Process %d not found.\n", int_id);
            return;
        }
    }

    // // Disallow sending message to oneself
    // running = Running_get();
    // if (running->id == int_id)
    // {
    //     printf("ERROR: Send failed. Cannot send a message to oneself.");
    //     return;
    // }
    
    // Disallow multiple messages
    if (result_search->msg.text[0] != '\0')
    {
        printf("ERROR: Process %d's message field is full.\n", int_id);
        return;
    }

    printf("Please enter the message (max 40 chars) to send to Process %d.\n", int_id);
    fgets(msg, sizeof(msg), stdin);

    // overkill error checking to prevent buffer overflow
    // (may not be exactly 40 chars, but close enough)
    char c;
    bool overflow = true;
    for (int i = 0; i < sizeof(msg); i++)
    {
        // Look for presence of a newline char
        if (msg[i] == '\n')
        {
            overflow = false;
        }
    }
    if (overflow == true)
    {
        while ((c = getchar()) != '\n' && c != EOF)
        {}
    }

    running = Running_get();
    if (running->id != INIT)
    {
        Queue_dequeue(Queue_get(running->priority));
        running->status = "BLOCKED";
        running->ready_queue = send_blocked;
        Queue_enqueue(send_blocked, running);
    }

    strncpy(result_search->msg.text, msg, sizeof(result_search->msg.text)); 
    result_search->msg.sender_id = running->id;
    puts("Message successfully sent.");
    if (running->id != INIT)
    {
        printf("Process %d has been send blocked while awaiting a reply.\n", \
        running->id);
    }
    // Case: Chosen process is already on receive_blocked q (premature receive)
    if (result_search->ready_queue == receive_blocked)
    {
        Pcb *removed = Queue_remove(receive_blocked, result_search->id);
        Queue_enqueue(Queue_get(removed->priority), removed);
        printf("Process %d received the message, and has been unblocked and re-enqueued into Ready Queue %d.\n",\
        removed->id, removed->priority);
        removed->status = "READY";
        removed->ready_queue = Queue_get(removed->priority);
    }

    Running_update();
}

void Ipc_receive()
{
    running = Running_get();
    // Case 1: premature receive
    if (running->msg.text[0] == '\0')
    {
        if (running->id == INIT)
        {
            puts("Process INIT tried to receive, but no message has been sent yet.");
            puts("This Receive call will be remembered.");
        }
        else
        {
        Queue_dequeue(running->ready_queue);
        Queue_enqueue(receive_blocked, running);
        running->status = "BLOCKED";
        running->ready_queue = receive_blocked;
        printf("Process %d tried to receive, but no message has been sent yet.\n", \
        running->id);
        printf("Process %d has been receive blocked while awaiting a message.\n", \
        running->id);
        Running_update();
        }

        running->msg.received = true;
    }

    // Case 2: receiving when you already have a message
    else
    {
        // if (running->id == INIT)
        // {
        //     puts("Message received from Process INIT:");
        // }
        printf("Message received from Process %d:\n", running->msg.sender_id);
        puts("~~~~~~~~~~~~~~~~~\n");
        printf("%s\n", running->msg.text);
        puts("~~~~~~~~~~~~~~~~~");
        // Reset message.
        running->msg.text[0] = '\0';
        running->msg.received = false;
    }
}

void Ipc_reply()
{
    puts("Please enter the ID of the Process you want to reply to.");
    fgets(str_id, sizeof(str_id), stdin);

    for (int i = 0; i < sizeof(str_id); i++)
    {
        if (isdigit(str_id[i]) == 0)
        {
            if (str_id[i] == '-' || str_id[i] == '\n' || str_id[i] == '\0') 
            {
                continue;
            }
            puts("ERROR: Invalid input. Must be a number.");
            return;
        }
    }
    sscanf(str_id, "%d", &int_id);
    
    search_result = Queue_search(send_blocked, int_id);

    if (search_result == NULL)
    {
        if (int_id == -1)
        {
            search_result = Init_get();
        }
        else 
        {
            printf("ERROR: Process %d is not waiting for a reply, or it does not exist.\n", int_id);
            return;
        }
    }

    printf("Please enter a reply (max 40 chars) to Process %d.\n", int_id);
    fgets(msg, sizeof(msg), stdin);

    // overkill error checking to prevent buffer overflow
    // (may not be exactly 40 chars, but close enough)
    char c;
    bool overflow = true;
    for (int i = 0; i < sizeof(msg); i++)
    {
        // Look for presence of a newline char
        if (msg[i] == '\n')
        {
            overflow = false;
        }
    }
    if (overflow == true)
    {
        while ((c = getchar()) != '\n' && c != EOF)
        {}
    }

    strncpy(search_result->msg.text, msg, sizeof(search_result->msg.text));
    puts("Reply successfully sent.");

    running = Running_get();
    // Set up the field to automatically print out reply next time it's running
    search_result->msg.sender_id = running->id;
    search_result->msg.received = true;

    if (search_result->id != INIT)
    {
        Pcb *removed = Queue_remove(send_blocked, int_id);
        Queue_enqueue(Queue_get(removed->priority), removed);
        printf("Process %d received the reply, and has been unblocked and re-enqueued into Ready Queue %d.\n", \
        removed->id, removed->priority);
        removed->status = "READY";
        removed->ready_queue = Queue_get(removed->priority);
    }

    // Edge case: same as Semaphore V().
    if (running->id == INIT)
    {
        Running_update();
    }
}

// Exclusively used in Pcb_kill().
Pcb *Ipc_search_all_blocked(int id)
{
    Pcb *search_result = NULL;
    search_result = Queue_search(send_blocked, id);
    if (search_result != NULL)
    {
        return search_result;
    }
    else
    {
        search_result = Queue_search(receive_blocked, id);
        return search_result;
    }
}

void Ipc_print_blocked(char *queue_type)
{
    printf("%s-Blocked Queue: \n", queue_type);
    if (List_count(send_blocked) == 0)
    {
        return;
    }
    Pcb *ptr = List_last(send_blocked);
    while (ptr != NULL)
    {
        printf("| P%d ", ptr->id);
        ptr = List_prev(send_blocked);
    }
    puts("|");
}

void Ipc_print_all_blocked()
{
    printf("Send-Blocked Queue: ");
    Pcb *ptr = List_last(send_blocked);
    while (ptr != NULL)
    {
        printf("| P%d ", ptr->id);
        ptr = List_prev(send_blocked);
    }
    if (List_count(send_blocked) != 0) {
        printf("|");
    }
    puts("");

    printf("Receive-Blocked Queue: ");
    ptr = List_last(receive_blocked);
    while (ptr != NULL)
    {
        printf("| P%d ", ptr->id);
        ptr = List_prev(receive_blocked);
    }
    if (List_count(receive_blocked) != 0) {
        printf("|");
    }
    puts("");
}

void Ipc_shutdown()
{
    List_free(send_blocked, free);
    List_free(receive_blocked, free);
}