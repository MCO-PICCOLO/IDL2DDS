#include <iostream>
#include <string>
#include <map>
#include <functional>
#include <yaml-cpp/yaml.h>

#include "dds/dds.h"
#include "publisher.h"

// Include generated IDL headers
#include "ADASObstacleDetection.h"
#include "BodyLightsHeadLamp.h"

// Map of topic names to their DDS entities
struct TopicContext {
    dds_entity_t participant;
    dds_entity_t topic;
    dds_entity_t writer;
    void* type_ptr;     // Pointer to the current data instance
};

static std::map<std::string, TopicContext> topic_contexts;

// DDS 시스템 초기화
bool initialize_dds() {
    std::cout << "Initializing DDS system..." << std::endl;
    // 기본적인 DDS 설정은 이미 토픽별 초기화에서 수행됩니다.
    // 여기서는 글로벌 설정이나 공통 리소스를 초기화할 수 있습니다.
    return true;
}

// DDS 시스템 정리
void cleanup_dds() {
    std::cout << "Cleaning up DDS system..." << std::endl;
    
    // 모든 토픽의 리소스 정리
    for (auto& ctx_pair : topic_contexts) {
        const std::string& topic_name = ctx_pair.first;
        TopicContext& ctx = ctx_pair.second;
        
        std::cout << "Cleaning up resources for topic: " << topic_name << std::endl;
        
        // 메모리 해제 (토픽 타입에 따라 다르게 처리)
        if (topic_name == "ADASObstacleDetectionIsWarning") {
            ADASObstacleDetectionIsWarning_free(
                static_cast<ADASObstacleDetectionIsWarning*>(ctx.type_ptr), DDS_FREE_ALL);
        }
        else if (topic_name == "HeadLampStatus") {
            BodyLightsHeadLampStatus_free(
                static_cast<BodyLightsHeadLampStatus*>(ctx.type_ptr), DDS_FREE_ALL);
        }
        
        // DDS 엔티티 삭제
        if (ctx.writer > 0) dds_delete(ctx.writer);
        if (ctx.topic > 0) dds_delete(ctx.topic);
        if (ctx.participant > 0) dds_delete(ctx.participant);
    }
    
    // 컨테이너 비우기
    topic_contexts.clear();
}

// Function to initialize DDS publisher for a specific topic
bool initialize_dds_publisher(const std::string& topic_name) {
    TopicContext ctx;
    ctx.participant = dds_create_participant(DDS_DOMAIN_DEFAULT, NULL, NULL);
    if (ctx.participant < 0) {
        std::cerr << "Error creating participant for topic " << topic_name << std::endl;
        return false;
    }
    
    // Create topic and writer based on topic name
    if (topic_name == "ADASObstacleDetectionIsWarning") {
        // 최신 CycloneDDS API에 맞게 직접 topic descriptor 사용
        ctx.topic = dds_create_topic(ctx.participant, &ADASObstacleDetectionIsWarning_desc, 
            topic_name.c_str(), NULL, NULL);
        if (ctx.topic < 0) return false;
        
        ctx.writer = dds_create_writer(ctx.participant, ctx.topic, NULL, NULL);
        if (ctx.writer < 0) return false;
        
        ctx.type_ptr = ADASObstacleDetectionIsWarning__alloc();
    }
    else if (topic_name == "HeadLampStatus") {
        // 최신 CycloneDDS API에 맞게 직접 topic descriptor 사용
        ctx.topic = dds_create_topic(ctx.participant, &BodyLightsHeadLampStatus_desc, 
            topic_name.c_str(), NULL, NULL);
        if (ctx.topic < 0) return false;
        
        ctx.writer = dds_create_writer(ctx.participant, ctx.topic, NULL, NULL);
        if (ctx.writer < 0) return false;
        
        ctx.type_ptr = BodyLightsHeadLampStatus__alloc();
    }
    // Add similar blocks for other topic types
    else {
        std::cerr << "Unknown topic type: " << topic_name << std::endl;
        return false;
    }
    
    topic_contexts[topic_name] = ctx;
    return true;
}

// Function to publish data for a specific topic
bool publish_data(const std::string& topic_name, const YAML::Node& data_node, int data_index) {
    if (topic_contexts.find(topic_name) == topic_contexts.end()) {
        std::cerr << "Topic not initialized: " << topic_name << std::endl;
        return false;
    }
    
    TopicContext& ctx = topic_contexts[topic_name];
    
    // Set data based on topic type
    if (topic_name == "ADASObstacleDetectionIsWarning") {
        ADASObstacleDetectionIsWarning* data = 
            static_cast<ADASObstacleDetectionIsWarning*>(ctx.type_ptr);
        data->value = data_node["value"].as<bool>();
        
        std::cout << "Publishing ADASObstacleDetectionIsWarning: value=" 
                  << (data->value ? "true" : "false") << std::endl;
                  
        dds_write(ctx.writer, data);
    }
    else if (topic_name == "HeadLampStatus") {
        BodyLightsHeadLampStatus* data = static_cast<BodyLightsHeadLampStatus*>(ctx.type_ptr);
        
        // HeadLampStatus 필드를 BodyLightsHeadLampStatus 구조체에 맞게 변경
        data->command = data_node["leftLampOn"].as<bool>() ? 1 : 0;
        data->status = data_node["rightLampOn"].as<bool>() ? 2 : 0;
        data->uistatus = data_node["highBeamOn"].as<bool>() ? 2 : 1;
        data->progress = 100; // 작업 완료 상태
        
        std::cout << "Publishing HeadLampStatus: command=" << data->command 
                  << ", status=" << data->status
                  << ", uistatus=" << data->uistatus << std::endl;
                  
        dds_write(ctx.writer, data);
    }
    // Add similar blocks for other topic types
    else {
        std::cerr << "Unknown topic type: " << topic_name << std::endl;
        return false;
    }
    
    return true;
}