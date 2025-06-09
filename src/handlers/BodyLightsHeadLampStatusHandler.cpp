#include <iostream>
#include <memory>
#include <string>
#include <yaml-cpp/yaml.h>
#include "dds/dds.h"
#include "TopicHandler.h"
#include "TopicHandlerRegistry.h"

// Include the generated DDS code - generated in build directory
#include "BodyLightsHeadLampStatus.h"

class BodyLightsHeadLampStatusHandler : public TopicHandler {
private:
    dds_entity_t writer = 0;
    dds_entity_t topic = 0;
    std::unique_ptr<BodyLightsHeadLampStatus> sample;

public:
    BodyLightsHeadLampStatusHandler() {
        sample = std::make_unique<BodyLightsHeadLampStatus>();
    }

    ~BodyLightsHeadLampStatusHandler() override {
        cleanup();
    }

    std::string getTopicName() const override {
        return "BodyLightsHeadLampStatus";
    }

    bool initialize(dds_entity_t participant) override {
        if (participant <= 0) {
            std::cerr << "Invalid participant entity" << std::endl;
            return false;
        }

        // Create topic
        topic = dds_create_topic(
            participant, 
            &BodyLightsHeadLampStatus_desc, 
            "BodyLightsHeadLampStatus", 
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

        std::cout << "BodyLightsHeadLampStatus publisher initialized" << std::endl;
        return true;
    }

    bool publish(const YAML::Node& data_node, int data_index) override {
        if (!sample) {
            std::cerr << "Sample not initialized" << std::endl;
            return false;
        }

        // Extract data from YAML
        try {
            // TODO: Extract fields from YAML and assign to sample
            // Example: sample->field1 = data_node["field1"].as<int32_t>();
            //          sample->field2 = data_node["field2"].as<bool>();
        } catch (const YAML::Exception& e) {
            std::cerr << "Error parsing YAML for BodyLightsHeadLampStatus: " << e.what() << std::endl;
            return false;
        }

        // Publish data
        dds_return_t ret = dds_write(writer, sample.get());
        if (ret < 0) {
            std::cerr << "Error writing BodyLightsHeadLampStatus sample: " << ret << std::endl;
            return false;
        }

        std::cout << "Published BodyLightsHeadLampStatus: "
                  // TODO: Add field values for logging
                  // Example: << "field1=" << sample->field1
                  std::endl;
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
REGISTER_TOPIC_HANDLER(BodyLightsHeadLampStatusHandler)
