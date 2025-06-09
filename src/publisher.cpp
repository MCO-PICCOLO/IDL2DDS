#include <iostream>
#include <string>
#include <map>
#include <memory>
#include <functional>
#include <yaml-cpp/yaml.h>

#include "dds/dds.h"
#include "publisher.h"
#include "TopicHandler.h"
#include "TopicHandlerRegistry.h"

// DDS 참가자 엔티티
static dds_entity_t g_participant = 0;

// 토픽 핸들러 관리를 위한 맵
static std::map<std::string, std::unique_ptr<TopicHandler>> topic_handlers;

// DDS 시스템 초기화
bool initialize_dds() {
    std::cout << "Initializing DDS system..." << std::endl;
    
    // 글로벌 DDS 참가자 생성
    g_participant = dds_create_participant(DDS_DOMAIN_DEFAULT, NULL, NULL);
    if (g_participant < 0) {
        std::cerr << "Error creating DDS participant" << std::endl;
        return false;
    }
    
    return true;
}

// DDS 시스템 정리
void cleanup_dds() {
    std::cout << "Cleaning up DDS system..." << std::endl;
    
    // 모든 토픽 핸들러의 리소스 정리
    for (auto& handler_pair : topic_handlers) {
        const std::string& topic_name = handler_pair.first;
        std::cout << "Cleaning up resources for topic: " << topic_name << std::endl;
        
        handler_pair.second->cleanup();
    }
    
    // 핸들러 맵 비우기
    topic_handlers.clear();
    
    // 글로벌 DDS 참가자 삭제
    if (g_participant > 0) {
        dds_delete(g_participant);
        g_participant = 0;
    }
}

// Function to initialize DDS publisher for a specific topic
bool initialize_dds_publisher(const std::string& topic_name) {
    // 이미 초기화된 토픽인지 확인
    if (topic_handlers.find(topic_name) != topic_handlers.end()) {
        std::cout << "Topic " << topic_name << " already initialized" << std::endl;
        return true;
    }

    // 레지스트리에서 토픽 핸들러 생성
    std::unique_ptr<TopicHandler> handler(TopicHandlerRegistry::getInstance().createHandler(topic_name));
    
    if (!handler) {
        std::cerr << "Unknown topic type: " << topic_name << std::endl;
        return false;
    }
    
    // 핸들러 초기화
    if (!handler->initialize(g_participant)) {
        std::cerr << "Failed to initialize handler for topic: " << topic_name << std::endl;
        return false;
    }
    
    // 초기화된 핸들러 저장
    topic_handlers[topic_name] = std::move(handler);
    std::cout << "Topic handler initialized for: " << topic_name << std::endl;
    
    return true;
}

// Function to publish data for a specific topic
bool publish_data(const std::string& topic_name, const YAML::Node& data_node, int data_index) {
    // 토픽이 초기화되었는지 확인
    auto it = topic_handlers.find(topic_name);
    if (it == topic_handlers.end()) {
        std::cerr << "Topic not initialized: " << topic_name << std::endl;
        return false;
    }
    
    // 해당 토픽 핸들러를 사용하여 데이터 발행
    return it->second->publish(data_node, data_index);
}