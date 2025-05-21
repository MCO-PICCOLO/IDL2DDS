#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <dds/dds.h>
#include "publisher.h"
#include "ADASObstacleDetection.h"  // Include the generated header

// DDS entities
static dds_entity_t participant;
static dds_entity_t publisher;
static dds_entity_t topic;
static dds_entity_t writer;

// Message structures
static ObstacleDetectionInternal internal_msg;  // Our internal representation
static ADASObstacleDetectionIsWarning dds_msg;  // DDS type generated from IDL

int publisher_init(const char *topic_name) {
    dds_return_t ret;
    dds_qos_t *qos;

    // Create the participant
    participant = dds_create_participant(DDS_DOMAIN_DEFAULT, NULL, NULL);
    if (participant < 0) {
        fprintf(stderr, "Failed to create participant: %s\n", dds_strretcode(-participant));
        return -1;
    }

    // Create a reliable QoS
    qos = dds_create_qos();
    dds_qset_reliability(qos, DDS_RELIABILITY_RELIABLE, DDS_SECS(10));

    // Create the publisher
    publisher = dds_create_publisher(participant, NULL, NULL);
    if (publisher < 0) {
        fprintf(stderr, "Failed to create publisher: %s\n", dds_strretcode(-publisher));
        dds_delete_qos(qos);
        return -1;
    }

    // Create the topic using the exact topic name "ADASObstacleDetectionIsWarning"
    // The descriptor name might be different based on how idlc generates the code
    topic = dds_create_topic(participant, &ADASObstacleDetectionIsWarning_desc, "ADASObstacleDetectionIsWarning", qos, NULL);
    if (topic < 0) {
        fprintf(stderr, "Failed to create topic: %s\n", dds_strretcode(-topic));
        dds_delete_qos(qos);
        return -1;
    }

    // Create the writer
    writer = dds_create_writer(publisher, topic, qos, NULL);
    if (writer < 0) {
        fprintf(stderr, "Failed to create writer: %s\n", dds_strretcode(-writer));
        dds_delete_qos(qos);
        return -1;
    }

    // Cleanup QoS
    dds_delete_qos(qos);

    printf("DDS Publisher initialized successfully\n");
    return 0;
}

void publisher_publish_obstacle_detection(bool warning_enabled) {
    if (writer < 0) {
        fprintf(stderr, "Writer not initialized\n");
        return;
    }

    // Store in internal message structure
    internal_msg.value = warning_enabled;

    // Convert to DDS message format - fields ordered according to IDL definition
    dds_msg.value = warning_enabled;

    // Write the message
    dds_return_t ret = dds_write(writer, &dds_msg);
    if (ret != DDS_RETCODE_OK) {
        fprintf(stderr, "Failed to publish message: %s\n", dds_strretcode(-ret));
    } else {
        printf("Published ADAS Obstacle Detection Warning: value=%s\n", 
               warning_enabled ? "true" : "false");
    }
}

void publisher_fini(void) {
    printf("Cleaning up DDS resources...\n");
    dds_delete(writer);
    dds_delete(topic);
    dds_delete(publisher);
    dds_delete(participant);
}

int publisher_run(void) {
    int ret;
    int count = 0;
    
    // Initialize the publisher with the topic name "ADASObstacleDetectionIsWarning"
    ret = publisher_init("ADASObstacleDetectionIsWarning");
    if (ret != 0) {
        return ret;
    }
    
    printf("Publisher running. Press Ctrl+C to stop.\n");
    
    // Main publishing loop
    while (1) {
        // Toggle the obstacle detection warning every 5 seconds
        bool warning_enabled = (count / 5) % 2 == 0;  // Toggle every 5 counts
        
        // Publish obstacle detection warning status
        publisher_publish_obstacle_detection(warning_enabled);
        
        count++;
        sleep(1);  // Send a message every second
    }
    
    // Cleanup (this won't be reached unless loop is broken)
    publisher_fini();
    return 0;
}