// TODO: valgrind reporting some errors?

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"
#include "pcb.h"
#include "ipc.h"
#include "semaphore.h"
#include "info.h"
#include "init.h"
#include "running.h"

static void print_options()
{
    puts("-------------------------------------------");
    puts("Please choose one of the following inputs: ");
    puts("-------------------------------------------");
    puts("C - create a process");
    puts("F - fork the current process");
    puts("K - kill a process");
    puts("E - kill the current process");
    puts("Q - expire the current process");
    puts("S - send a message to another process");
    puts("R - receive a message from another process");
    puts("Y - reply to another process");
    puts("N - create a new semaphore");
    puts("P - call P(S) on a semaphore");
    puts("V - call V(S) on a semaphore");
    puts("I - info about a process");
    puts("T - total info about all processes\n");
}

char command[3];

int main()
{
    // Initialize
    Init_init();
    Running_init();
    Queue_init();
    Ipc_init();
    Semaphore_init();

    puts("\nWelcome to the Process Scheduling Simulation.\n");

    while (1)
    {
        print_options();
        fgets(command, sizeof(command), stdin);
        switch (command[0])
        {
            case 'c':
                Pcb_create();
                break;
            case 'f':
                Pcb_fork();
                break;
            case 'k':
                Pcb_kill();
                break;
            case 'e':
                Pcb_exit();
                break;
            case 'q':
                Pcb_quantum();
                break;
            case 's':
                Ipc_send();
                break;
            case 'r':
                Ipc_receive();
                break;
            case 'y':
                Ipc_reply();
                break;
            case 'n':
                Semaphore_create();
                break;
            case 'p':
                Semaphore_P();
                break;
            case 'v':
                Semaphore_V();
                break;
            case 'i':
                Info_procinfo();
                break;
            case 't':
                Info_totalinfo();
                break;
            default:
                puts("ERROR: Invalid input.");
                break;
        }   
    }
}