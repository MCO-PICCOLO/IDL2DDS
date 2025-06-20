#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <map>
#include <vector>
#include <string>
#include <filesystem>
#include <atomic>
#include <csignal>
#include <yaml-cpp/yaml.h>

#include "dds/dds.h"
#include "publisher.h"

// Forward declarations
struct TopicConfig;
void run_test_for_topic(const std::string& topic_name, const TopicConfig& config);
bool initialize_dds_publisher(const std::string& topic_name);
bool publish_data(const std::string& topic_name, const YAML::Node& data_node, int data_index);

// Signal handler flag
std::atomic<bool> running{true};

// Structure to hold topic configuration
struct TopicConfig {
    int start_time;
    int end_time;
    double frequency;
    std::vector<YAML::Node> data_values;
};

// 필요한 함수 선언들 (다른 파일에 정의되어 있을 수 있는 함수)
bool initialize_dds();
void cleanup_dds();

// Main function
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <test-file.yaml>" << std::endl;
        return 1;
    }

    std::string test_file = argv[1];
    
    // Check if the file exists in test-cases directory if not an absolute path
    if (!std::filesystem::exists(test_file)) {
        std::string test_cases_path = "test-cases/" + test_file;
        if (std::filesystem::exists(test_cases_path)) {
            test_file = test_cases_path;
        } else {
            std::cerr << "Test file not found: " << test_file << std::endl;
            return 1;
        }
    }

    std::cout << "Loading test configuration from: " << test_file << std::endl;
    
    // Parse YAML file
    YAML::Node config;
    try {
        config = YAML::LoadFile(test_file);
    } catch (const YAML::Exception& e) {
        std::cerr << "Error parsing YAML file: " << e.what() << std::endl;
        return 1;
    }

    // Setup signal handler
    std::signal(SIGINT, [](int) { running = false; });

    // Store topic configurations
    std::map<std::string, TopicConfig> topics;

    // Process each topic in the YAML
    for (const auto& topic_it : config) {
        // Skip non-topic entries (like version)
        if (topic_it.first.IsScalar() && topic_it.first.as<std::string>() == "version") {
            continue;
        }

        std::string topic_name = topic_it.first.as<std::string>();
        const YAML::Node& topic_config = topic_it.second;

        TopicConfig tc;
        tc.start_time = topic_config["start_time"].as<int>(0);
        tc.end_time = topic_config["end_time"].as<int>(0);
        tc.frequency = topic_config["frequency"].as<double>(1.0);

        // Extract data values
        if (topic_config["data"]) {
            for (const auto& data : topic_config["data"]) {
                tc.data_values.push_back(data);
            }
        }

        topics[topic_name] = tc;
        std::cout << "Loaded configuration for topic: " << topic_name << std::endl;
    }

    // Initialize DDS system
    if (!initialize_dds()) {
        std::cerr << "Failed to initialize DDS" << std::endl;
        return 1;
    }

    // Start time for test
    auto start_time = std::chrono::steady_clock::now();
    
    // Start a thread for each topic
    std::vector<std::thread> topic_threads;
    for (const auto& topic_pair : topics) {
        topic_threads.emplace_back(run_test_for_topic, 
                                  topic_pair.first, 
                                  topic_pair.second);
    }

    // Join all threads
    for (auto& thread : topic_threads) {
        thread.join();
    }

    // Cleanup DDS
    cleanup_dds();
    
    return 0;
}

// Function to run test for a specific topic
void run_test_for_topic(const std::string& topic_name, const TopicConfig& config) {
    std::cout << "Starting publisher for topic: " << topic_name << std::endl;
    
    // Initialize DDS publisher for this topic
    if (!initialize_dds_publisher(topic_name)) {
        std::cerr << "Failed to initialize publisher for topic: " << topic_name << std::endl;
        return;
    }

    auto start_time = std::chrono::steady_clock::now();
    
    // Wait until start time
    if (config.start_time > 0) {
        std::this_thread::sleep_for(std::chrono::seconds(config.start_time));
    }
    
    // Calculate sleep duration between messages
    auto sleep_duration = std::chrono::microseconds(
        static_cast<int64_t>(1000000.0 / config.frequency));
    
    int data_index = 0;
    int total_data = config.data_values.size();
    
    // Continue until end time (if specified) or until program is terminated
    while (running) {
        auto current_time = std::chrono::steady_clock::now();
        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(
            current_time - start_time).count();
        
        // Check if we've reached the end time
        if (config.end_time > 0 && elapsed_seconds >= config.end_time) {
            break;
        }
        
        // Publish current data value
        if (total_data > 0) {
            publish_data(topic_name, config.data_values[data_index], data_index);
            
            // Move to next data value in sequence
            data_index = (data_index + 1) % total_data;
        }
        
        // Sleep until next publish time
        std::this_thread::sleep_for(sleep_duration);
    }
    
    std::cout << "Stopping publisher for topic: " << topic_name << std::endl;
}