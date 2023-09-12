#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "info.h"
#include "pcb.h"
#include "queue.h"
#include "semaphore.h"
#include "ipc.h"
#include "running.h"
#include "init.h"

static int int_id;
static char str_id[6];

void Info_procinfo()
{
    puts("Please enter the ID of the Process whose information you want.");
    fgets(str_id, sizeof(str_id), stdin);

    // This snippet makes it possible to print info about INIT 
    // (whose ID is arbitrarily set to -1).
    if (strncmp(str_id, "-1", 2) == 0) 
    {
        Pcb_print(Init_get());
        return;
    }

    // Normal case
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
        return;
    }
    else
    {        
        puts("--------------------------------------");
        printf("Displaying information for Process %d.\n", int_id);
        puts("--------------------------------------");
        Pcb_print(result_search);
    }
}

void Info_totalinfo()
{
    puts("------------------------------------------");
    puts("Displaying contents of all Process Queues.");
    puts("------------------------------------------");
    Queue_print_all();
    puts("");
    Semaphore_print_all_blocked();
    puts("");
    Ipc_print_all_blocked();
}