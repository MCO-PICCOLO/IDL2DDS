#ifndef PUBLISHER_H
#define PUBLISHER_H

#include <dds/dds.h>
#include <stdbool.h>
#include "common_types.h"

// Function to initialize the DDS publisher
int publisher_init(const char *topic_name);

// Function to publish obstacle detection warning message
void publisher_publish_obstacle_detection(bool warning_enabled);

// Function to clean up the publisher resources
void publisher_fini(void);

// Function to run the publisher in a loop (for main.c)
int publisher_run(void);

#endif // PUBLISHER_H