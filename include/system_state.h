#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

#include <Arduino.h>

typedef enum {
    STATE_IDLE,
    STATE_CARD_DETECTED,
    STATE_UNLOCK,
    STATE_ERROR
} system_state_t;

void state_init();
void state_update();

#endif