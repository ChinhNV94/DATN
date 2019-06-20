#ifndef ZIGBEE_H
#define ZIGBEE_H

#include "myDataType.h"

#define MAX_SIZE_RX_BUFFER  255
#define MAX_SIZE_TX_BUFFER  255

#define BEACON              0x00
#define DATA                0x01
#define ACK                 0x02
#define COMMAND             0x03

#define UNICAST             0x01
#define MULTICAST           0x02
#define BROADCAST           0xFF

typedef struct _uint16
{
    uint8 msb;
    uint8 lsb;
}uint16_type;

typedef struct _frame_ctrl
{
    char* label;
    uint16_type val;
}frame_ctrl_type;

enum command_frame_id
{
    association_request = 1,
    association_response,
    disassociation_notification,
    data_request,
    PAN_ID_confilict,
    orphan_notification,
    beacon_request,
    coordinator_realignment,
    gts_request
};

typedef struct _header
{
    /* uint8 start; */
    uint8 data_length;
    uint16_type frame_ctrl;
    uint16_type dest_address;
    uint16_type source_address;
}header_type;

typedef struct _payload
{
    uint8 *data;
    /*uint8 cs;
    uint8 end;*/
}payload_type;

typedef struct _frame
{
    header_type header;
    payload_type payload;
}frame_type;

extern uint8 rx_address_count;

extern uint16_type rx_address[100];
extern frame_type rx_frame;
extern frame_type tx_frame;
extern uint8* tx_data;

uint8 cs_calculation(frame_type frame);

void get_RxHeader();
frame_ctrl_type check_frame_ctrl();
void get_RxPayload();
void delete_RxFrame();
uint8 get_RxFrame();
void set_TxHeader(uint8 len, uint16_type ctrl, uint16_type dest_add, uint16_type source_add);
void set_TxPayload(uint8 data[]);
void set_TxFrame(uint8 len, uint16_type ctrl, uint16_type dest_add, uint16_type source_add, uint8 data[]);
void putTxFrameToTxBuffer();

void ZigbeeTask();

#endif
