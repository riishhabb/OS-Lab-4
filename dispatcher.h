#ifndef DISPATCHER_H
#define DISPATCHER_H

#define MAX_PROCESSES 1000
#define MEMORY_SIZE 1024
#define RT_MEMORY_RESERVE 64
#define USER_MEMORY_LIMIT (MEMORY_SIZE - RT_MEMORY_RESERVE)

typedef struct {
    int arrival_time;
    int priority;
    int processor_time;
    int memory;
    int printers;
    int scanners;
    int modems;
    int cds;
    int pid;
} Process;

void load_dispatch_list(const char *filename);
void initialize_resources();
void dispatcher_tick();
int all_processes_completed();

#endif
