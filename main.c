#include <stdio.h>
#include <stdlib.h>
#include "dispatcher.h"

int main() {
    char filename[100];
    printf("Enter path to dispatch list file: ");
    scanf("%s", filename);

    load_dispatch_list(filename);     // Parses input file
    initialize_resources();           // Initialize memory, IO resources, queues

    while (!all_processes_completed()) {
        dispatcher_tick();           // Simulate one second tick
    }

    printf("\nAll processes completed.\n");
    return 0;
}
