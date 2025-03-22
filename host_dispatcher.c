// host_dispatcher.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 1000
#define MEMORY_SIZE 1024
#define RT_MEMORY_RESERVE 64
#define USER_MEMORY_LIMIT (MEMORY_SIZE - RT_MEMORY_RESERVE)

// Process structure
typedef struct {
    int arrival_time, priority, processor_time, memory;
    int printers, scanners, modems, cds;
    int pid;
    int remaining_time;
} Process;

// Memory block structure
typedef struct {
    int size;
    int start_address;
} Block;

// Globals
Process dispatch_list[MAX_PROCESSES];
int total_processes = 0;
int system_time = 0;

Block free_blocks[MEMORY_SIZE];
int block_count = 0;

int available_printers = 2, available_scanners = 1;
int available_modems = 1, available_cds = 2;

int running_pid = -1;

// Function prototypes
void load_dispatch_list(const char *filename);
void insert_block(Block b);
int allocate_memory(int size, int *start_addr);
void free_memory(int start_addr, int size);
int resources_available(Process p);
void allocate_resources(Process p);
void free_resources(Process p);
void dispatcher_tick();
int all_processes_completed();

// Load dispatch list from file
void load_dispatch_list(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("File error");
        exit(1);
    }
    while (fscanf(f, "%d, %d, %d, %d, %d, %d, %d, %d\n",
                  &dispatch_list[total_processes].arrival_time,
                  &dispatch_list[total_processes].priority,
                  &dispatch_list[total_processes].processor_time,
                  &dispatch_list[total_processes].memory,
                  &dispatch_list[total_processes].printers,
                  &dispatch_list[total_processes].scanners,
                  &dispatch_list[total_processes].modems,
                  &dispatch_list[total_processes].cds) == 8) {
        dispatch_list[total_processes].pid = total_processes;
        dispatch_list[total_processes].remaining_time = dispatch_list[total_processes].processor_time;
        total_processes++;
    }
    fclose(f);
}

// Memory Management - Best Fit
void insert_block(Block b) {
    free_blocks[block_count++] = b;
}

int allocate_memory(int size, int *start_addr) {
    int best_index = -1;
    for (int i = 0; i < block_count; i++) {
        if (free_blocks[i].size >= size) {
            if (best_index == -1 || free_blocks[i].size < free_blocks[best_index].size) {
                best_index = i;
            }
        }
    }
    if (best_index == -1) return 0;

    *start_addr = free_blocks[best_index].start_address;

    // Reduce block size or remove
    if (free_blocks[best_index].size > size) {
        free_blocks[best_index].start_address += size;
        free_blocks[best_index].size -= size;
    } else {
        for (int j = best_index; j < block_count - 1; j++)
            free_blocks[j] = free_blocks[j + 1];
        block_count--;
    }
    return 1;
}

void free_memory(int start_addr, int size) {
    Block b = {size, start_addr};
    insert_block(b);
}

// Resource Management
int resources_available(Process p) {
    return p.printers <= available_printers &&
           p.scanners <= available_scanners &&
           p.modems <= available_modems &&
           p.cds <= available_cds;
}

void allocate_resources(Process p) {
    available_printers -= p.printers;
    available_scanners -= p.scanners;
    available_modems -= p.modems;
    available_cds -= p.cds;
}

void free_resources(Process p) {
    available_printers += p.printers;
    available_scanners += p.scanners;
    available_modems += p.modems;
    available_cds += p.cds;
}

// Tick Simulation
void dispatcher_tick() {
    for (int i = 0; i < total_processes; i++) {
        Process *p = &dispatch_list[i];
        if (p->remaining_time > 0 && p->arrival_time <= system_time) {
            int start_addr;
            if (p->priority == 0 && allocate_memory(p->memory, &start_addr)) {
                p->remaining_time--;
                printf("Time %d: RT Process %d running. Memory: %dMB at %d\n", system_time, p->pid, p->memory, start_addr);
                if (p->remaining_time == 0) {
                    free_memory(start_addr, p->memory);
                    printf("Time %d: RT Process %d completed.\n", system_time, p->pid);
                }
                break;
            }
            if (p->priority > 0 && resources_available(*p) && allocate_memory(p->memory, &start_addr)) {
                allocate_resources(*p);
                p->remaining_time--;
                printf("Time %d: User Process %d running. Priority: %d, Memory: %dMB at %d\n", system_time, p->pid, p->priority, p->memory, start_addr);
                if (p->remaining_time == 0) {
                    free_resources(*p);
                    free_memory(start_addr, p->memory);
                    printf("Time %d: User Process %d completed.\n", system_time, p->pid);
                }
                break;
            }
        }
    }
    system_time++;
}

int all_processes_completed() {
    for (int i = 0; i < total_processes; i++) {
        if (dispatch_list[i].remaining_time > 0)
            return 0;
    }
    return 1;
}

// Main function
int main() {
    char file[100];
    printf("Enter path to dispatch list file: ");
    scanf("%s", file);
    load_dispatch_list(file);
    insert_block((Block){MEMORY_SIZE - RT_MEMORY_RESERVE, RT_MEMORY_RESERVE});

    while (!all_processes_completed()) {
        dispatcher_tick();
    }
    printf("\nAll processes completed.\n");
    return 0;
}
