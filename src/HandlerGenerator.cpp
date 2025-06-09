// This file ensures all topic handlers are included in the build
// Handlers are registered with the registry during static initialization

// Include all topic handlers
// Each handler should dynamically register itself with the TopicHandlerRegistry
// using the REGISTER_TOPIC_HANDLER macro

#include "handlers/ADASObstacleDetectionIsWarningHandler.cpp"
#include "handlers/BodyLightsHeadLampStatusHandler.cpp"
#include "handlers/VehicleStatusHandler.cpp"
// When adding a new IDL file:
// 1. Create a new handler class in src/handlers/[TopicName]Handler.cpp
// 2. Add an include for the new handler here
