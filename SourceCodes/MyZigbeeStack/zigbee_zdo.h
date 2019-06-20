#ifndef ZIGBEE_ZDO_H
#define ZIGBEE_ZDO_H

/*************************************************************************/
/* Include */

#include "zigbee.h"

/*************************************************************************/
/* Define & Typedef */

#define COORDINATOR

#if defined(COORDINATOR)
    #define ZIGBEE_NODE      0
#else
    #define ZIGBEE_NODE      1
#endif

/*************************************************************************/
/* Global variable */

/*extern address_type add_sensor[5];*/
extern uint16_type add_sensor[5];

extern data_16bit_label addRx[100];

#endif
