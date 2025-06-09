#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dds/dds.h>
#include "topic_publisher.h"
#include "ADASObstacleDetection.h" // Generated from IDL

static dds_entity_t participant;
static dds_entity_t publisher;
static dds_entity_t topic;
static dds_entity_t writer;

void init_dds(const char *topic_name) {
    dds_qos_t *qos;
    
    // Create the participant
    participant = dds_create_participant(DDS_DOMAIN_DEFAULT, NULL, NULL);
    if (participant < 0) {
        fprintf(stderr, "Failed to create participant: %s\n", dds_strretcode(-participant));
        return;
    }
    
    // Create QoS with reliable communication
    qos = dds_create_qos();
    dds_qset_reliability(qos, DDS_RELIABILITY_RELIABLE, DDS_SECS(10));
    
    // Create the publisher entity
    publisher = dds_create_publisher(participant, NULL, NULL);
    if (publisher < 0) {
        fprintf(stderr, "Failed to create publisher: %s\n", dds_strretcode(-publisher));
        dds_delete_qos(qos);
        return;
    }
    
    // Create the topic
    topic = dds_create_topic(participant, &ADASObstacleDetectionIsWarning_desc, topic_name, qos, NULL);
    if (topic < 0) {
        fprintf(stderr, "Failed to create topic: %s\n", dds_strretcode(-topic));
        dds_delete_qos(qos);
        return;
    }
    
    // Create the data writer
    writer = dds_create_writer(publisher, topic, qos, NULL);
    if (writer < 0) {
        fprintf(stderr, "Failed to create writer: %s\n", dds_strretcode(-writer));
        dds_delete_qos(qos);
        return;
    }
    
    // Clean up QoS
    dds_delete_qos(qos);
    
    printf("DDS Topic Publisher initialized for '%s'\n", topic_name);
}

void publish_message(const ADASObstacleDetectionIsWarning *data) {
    if (writer < 0) {
        fprintf(stderr, "DDS writer not initialized\n");
        return;
    }
    
    dds_return_t ret = dds_write(writer, data);
    if (ret != DDS_RETCODE_OK) {
        fprintf(stderr, "Error publishing message: %s\n", dds_strretcode(-ret));
    } else {
        printf("Published ADAS Obstacle Detection Warning: value=%s\n", 
               data->value ? "true" : "false");
    }
}

void cleanup_dds() {
    printf("Cleaning up DDS Topic Publisher resources...\n");
    dds_delete(writer);
    dds_delete(topic);
    dds_delete(publisher);
    dds_delete(participant);
    printf("DDS resources released\n");
}