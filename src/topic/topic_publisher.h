#ifndef TOPIC_PUBLISHER_H
#define TOPIC_PUBLISHER_H

#include "ADASObstacleDetection.h" // Generated from IDL
#include <string>

// C++ API로 변환된 함수들
void init_dds(const std::string& topic_name);
void publish_message(const ADASObstacleDetectionIsWarning *data);
void cleanup_dds();

#endif // TOPIC_PUBLISHER_H