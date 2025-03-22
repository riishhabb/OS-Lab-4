#include <stdio.h>
#include "dispatcher.h"

Process dispatch_list[MAX_PROCESSES];
int total_processes = 0;

void load_dispatch_list(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("File error");
        exit(1);
    }

    while (fscanf(file, "%d, %d, %d, %d, %d, %d, %d, %d\n",
                  &dispatch_list[total_processes].arrival_time,
                  &dispatch_list[total_processes].priority,
                  &dispatch_list[total_processes].processor_time,
                  &dispatch_list[total_processes].memory,
                  &dispatch_list[total_processes].printers,
                  &dispatch_list[total_processes].scanners,
                  &dispatch_list[total_processes].modems,
                  &dispatch_list[total_processes].cds) == 8) {
        dispatch_list[total_processes].pid = total_processes;
        total_processes++;
    }

    fclose(file);
}
