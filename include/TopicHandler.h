#pragma once

#include <string>
#include <yaml-cpp/yaml.h>
#include "dds/dds.h"

// 모든 토픽 핸들러가 구현해야 할 인터페이스
class TopicHandler {
public:
    virtual ~TopicHandler() = default;
    
    // 토픽 이름 반환
    virtual std::string getTopicName() const = 0;
    
    // DDS 퍼블리셔 초기화
    virtual bool initialize(dds_entity_t participant) = 0;
    
    // 데이터 발행
    virtual bool publish(const YAML::Node& data_node, int data_index) = 0;
    
    // 리소스 정리
    virtual void cleanup() = 0;
};

// 토픽 핸들러 팩토리 함수 타입 정의
using TopicHandlerCreator = TopicHandler* (*)();
