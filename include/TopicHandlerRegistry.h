#pragma once

#include <map>
#include <string>
#include <functional>
#include <vector>
#include "TopicHandler.h"

// 토픽 핸들러 레지스트리 클래스 - 모든 토픽 핸들러를 관리
class TopicHandlerRegistry {
public:
    static TopicHandlerRegistry& getInstance() {
        static TopicHandlerRegistry instance;
        return instance;
    }

    // 토픽 핸들러 등록
    void registerHandler(const std::string& topic_name, TopicHandlerCreator creator) {
        handlers_[topic_name] = creator;
    }

    // 토픽 핸들러 생성
    TopicHandler* createHandler(const std::string& topic_name) {
        auto it = handlers_.find(topic_name);
        if (it != handlers_.end()) {
            return it->second();
        }
        return nullptr;
    }

    // 등록된 토픽 이름 목록 반환
    std::vector<std::string> getRegisteredTopicNames() const {
        std::vector<std::string> names;
        for (const auto& pair : handlers_) {
            names.push_back(pair.first);
        }
        return names;
    }

private:
    TopicHandlerRegistry() = default;
    std::map<std::string, TopicHandlerCreator> handlers_;
};

// 자동 토픽 핸들러 등록을 위한 매크로
// 각 핸들러 클래스마다 고유한 registrar 변수 이름을 갖도록 함
#define REGISTER_TOPIC_HANDLER(TopicClass) \
    namespace { \
        TopicHandler* create##TopicClass() { \
            return new TopicClass(); \
        } \
        struct TopicClass##Registrar { \
            TopicClass##Registrar() { \
                TopicHandlerRegistry::getInstance().registerHandler( \
                    TopicClass().getTopicName(), create##TopicClass); \
            } \
        }; \
        static TopicClass##Registrar TopicClass##_registrar; \
    }
