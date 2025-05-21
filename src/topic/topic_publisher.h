#ifndef TOPIC_PUBLISHER_H
#define TOPIC_PUBLISHER_H

#include "common_types.h"
#include "ADASObstacleDetection.h" // Generated from IDL

// Function to initialize the DDS topic publisher
void init_dds(const char *topic_name);

// Function to publish a message on the topic
void publish_message(const ADASObstacleDetectionIsWarning *data);

// Function to clean up DDS resources
void cleanup_dds(void);

#endif // TOPIC_PUBLISHER_H