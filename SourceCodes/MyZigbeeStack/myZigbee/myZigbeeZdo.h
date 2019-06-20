#ifndef ZIGBEE_ZDO_H
#define ZIGBEE_ZDO_H

#include "MyZigbee.h"

#define COORDINATOR

#if defined(COORDINATOR)
    #define ZIGBEE_NODE      0
#else
    #define ZIGBEE_NODE      1
#endif

extern uint16_type add_sensor[5];

#endif
