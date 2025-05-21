#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <stdint.h>
#include <time.h>

// This is our internal structure - completely separate from the DDS generated one
typedef struct {
    int32_t progress;  // 0 ~ 100
    int32_t command;   // 0 = OFF, 1 = ON
    int32_t uistatus;  // 0 = IS_ACTING, 1 = HEADLIGHT_IS_OFF, 2 = HEADLIGHT_IS_ON
    int32_t status;    // 0 = IDLE, 1 = START, 2 = ACTING, 3 = END
} HeadLampStatusInternal;

// ADAS Obstacle Detection internal structure
typedef struct {
    bool value;  // True = Warning enabled, False = Warning disabled
} ObstacleDetectionInternal;

// Helper function to get current time as seconds since epoch
static inline int32_t get_current_timestamp(void) {
    return (int32_t)time(NULL);
}

#endif // COMMON_TYPES_H