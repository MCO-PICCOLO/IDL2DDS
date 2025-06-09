#ifndef PUBLISHER_H
#define PUBLISHER_H

#include <dds/dds.h>
#include <string>
#include <yaml-cpp/yaml.h>

// C++ API로 변환
bool initialize_dds(); // DDS 시스템 초기화
void cleanup_dds(); // DDS 시스템 정리

// C++ YAML 기반 publishing 함수들
bool initialize_dds_publisher(const std::string& topic_name);
bool publish_data(const std::string& topic_name, const YAML::Node& data_node, int data_index);

#endif // PUBLISHER_H