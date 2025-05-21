#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <dds/dds.h>
#include "publisher.h"

// Flag for graceful shutdown
static volatile int keep_running = 1;

// Signal handler for Ctrl+C
void signal_handler(int sig) {
    printf("\nCaught signal %d. Shutting down...\n", sig);
    keep_running = 0;
}

int main(int argc, char **argv) {
    // Set up signal handler for graceful termination
    signal(SIGINT, signal_handler);
    
    printf("DDS ADASObstacleDetectionIsWarning Sender Starting...\n");
    
    // Initialize the publisher
    if (publisher_init("ADASObstacleDetectionIsWarning") != 0) {
        fprintf(stderr, "Failed to initialize publisher\n");
        return EXIT_FAILURE;
    }
    
    printf("Publisher initialized. Sending obstacle detection warnings...\n");
    
    // Main loop - send messages until interrupted
    int count = 0;
    printf("Starting to publish messages. Press Ctrl+C to stop.\n");
    
    while (keep_running) {
        // Toggle the warning status every 5 seconds
        bool warning_enabled = (count / 5) % 2 == 0;  // Toggle every 5 counts
        
        printf("[%d] Sending obstacle detection warning: %s\n", 
               count, warning_enabled ? "true" : "false");
               
        // Publish obstacle detection warning status
        publisher_publish_obstacle_detection(warning_enabled);
        
        count++;
        fflush(stdout); // Ensure output is flushed immediately
        sleep(1);  // Wait for a second between messages
    }
    
    // Clean up
    publisher_fini();
    printf("DDS Sender terminated.\n");
    
    return EXIT_SUCCESS;
}