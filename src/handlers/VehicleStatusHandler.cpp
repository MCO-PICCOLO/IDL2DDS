#include <iostream>
#include <memory>
#include "dds/dds.h"

#ifndef VEHICLE_STATUS_HANDLER_INCLUDED
#define VEHICLE_STATUS_HANDLER_INCLUDED

// Include the generated DDS code
#include "VehicleStatus.h"

class VehicleStatusHandler : public TopicHandler {
private:
    dds_entity_t writer = 0;
    dds_entity_t topic = 0;
    std::unique_ptr<VehicleStatus> sample;

public:
    VehicleStatusHandler() {
        sample = std::make_unique<VehicleStatus>();
    }

    ~VehicleStatusHandler() override {
        cleanup();
    }

    std::string getTopicName() const override {
        return "VehicleStatus";
    }

    bool initialize(dds_entity_t participant) override {
        if (participant <= 0) {
            std::cerr << "Invalid participant entity" << std::endl;
            return false;
        }

        // Create topic
        topic = dds_create_topic(
            participant, 
            &VehicleStatus_desc, 
            "VehicleStatus", 
            NULL, 
            NULL
        );
        if (topic < 0) {
            std::cerr << "Error creating topic: " << topic << std::endl;
            return false;
        }

        // Create writer
        dds_qos_t* qos = dds_create_qos();
        dds_qset_reliability(qos, DDS_RELIABILITY_RELIABLE, DDS_SECS(10));
        writer = dds_create_writer(participant, topic, qos, NULL);
        dds_delete_qos(qos);

        if (writer < 0) {
            std::cerr << "Error creating writer: " << writer << std::endl;
            return false;
        }

        std::cout << "VehicleStatus publisher initialized" << std::endl;
        return true;
    }

    bool publish(const YAML::Node& data_node, int data_index) override {
        if (!sample) {
            std::cerr << "Sample not initialized" << std::endl;
            return false;
        }

        // Extract data from YAML
        try {
            sample->speed = data_node["speed"].as<int32_t>();
            sample->rpm = data_node["rpm"].as<int32_t>();
            sample->fuelLevel = data_node["fuelLevel"].as<float>();
            sample->engineOn = data_node["engineOn"].as<bool>();
            sample->gear = data_node["gear"].as<int32_t>();
        } catch (const YAML::Exception& e) {
            std::cerr << "Error parsing YAML for VehicleStatus: " << e.what() << std::endl;
            return false;
        }

        // Publish data
        dds_return_t ret = dds_write(writer, sample.get());
        if (ret < 0) {
            std::cerr << "Error writing VehicleStatus sample: " << ret << std::endl;
            return false;
        }

        std::cout << "Published VehicleStatus: speed=" << sample->speed
                  << ", rpm=" << sample->rpm
                  << ", fuelLevel=" << sample->fuelLevel
                  << ", engineOn=" << (sample->engineOn ? "true" : "false")
                  << ", gear=" << sample->gear << std::endl;
        return true;
    }

    void cleanup() override {
        if (writer > 0) {
            dds_delete(writer);
            writer = 0;
        }
        if (topic > 0) {
            dds_delete(topic);
            topic = 0;
        }
    }
};

// Register this handler with the registry
REGISTER_TOPIC_HANDLER(VehicleStatusHandler)
