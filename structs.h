#pragma once

enum {
    INIT = -1
};

typedef struct Message Msg;
struct Message {
    char text[40];
    int sender_id; 
    bool received;
};

typedef struct pcb {
    int id; 
    char *status;
    int priority;
    List *ready_queue;
    Msg msg;
} Pcb;

typedef struct ready_queue {
    float time;
    float percent_share;
} Ready_Queue;