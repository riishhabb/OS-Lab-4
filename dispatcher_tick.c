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
