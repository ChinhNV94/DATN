#ifndef ZIGBEE_H
#define ZIGBEE_H

/*************************************************************************/
/* Include */

#include "data_type.h"
#include "queue.h"

/*************************************************************************/
/* Define & Typedef */

#define MAX_SIZE_RX_BUFFER  255
#define MAX_SIZE_TX_BUFFER  255

#define COMMAND             0x00
#define ACK                 0x01
#define DATA                0x02
#define BEACON              0x03

#define REQUEST_DATA        0x04
#define REQUEST_ACK         0x05

#define UNICAST             0x00
#define MULTICAST           0x01
#define BROADCAST           0x02

#define NONE                0x00
#define ONRECEIVE           0x01
#define ONTRANSMIT          0x02

#define HEADER_LENGTH       7

#define TOTAL_NODE          100

typedef struct _uint16
{
    uint8 msb;
    uint8 lsb;
}uint16_type;

typedef struct _data_16bit_label
{
    char* label;
    uint16_type val;
}data_16bit_label;

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

/*************************************************************************/
/* Global variable */

/*extern uint8 RxBuffer[MAX_SIZE_RX_BUFFER];
extern uint8 TxBuffer[MAX_SIZE_TX_BUFFER];
extern uint16 rx_buffer_size;
extern uint16 tx_buffer_size;
extern uint16 rx_buffer_read_index;
extern uint16 tx_buffer_read_index;
extern uint16 rx_buffer_write_index;
extern uint16 tx_buffer_write_index;*/

extern uint8 zigbee_task;
extern queue RxQueue;
extern queue TxQueue;
extern uint8 rx_address_count;
extern uint16_type rx_address[100];
extern frame_type rx_frame;
extern frame_type tx_frame;

extern data_16bit_label frame_control[4];
extern char* CMD[3];
extern char* Mode[3];

extern uint8 Blank_Spots;

/*************************************************************************/
/* function */

/* uint8 cs_calculation(frame_type frame); */
void get_Rxheader();
data_16bit_label check_frame_ctrl();
char* get_CMD(frame_type fr);
char* get_MODE(frame_type fr);
void get_Rxpayload();
void delete_Rxframe();
uint8 get_Rxframe();
void setTxHeader(uint8 len, uint8 mode, uint8 cmd, uint16_type dest_add, uint16_type source_add);
void set_Txheader(uint8 len, uint16_type ctrl, uint16_type dest_add, uint16_type source_add);
void setTxpayload();
void set_Txpayload(uint8 data[]);
void setTxFrame(uint8 len, uint8 mode, uint8 cmd, uint16_type dest_add, uint16_type source_add, uint8 data[]);
void set_Txframe(uint8 len, uint16_type ctrl, uint16_type dest_add, uint16_type source_add, uint8 data[]);
void putTxFrameToTxBuffer();
void queue_putFrame(queue *q, frame_type fr);
void zigbeeTask();

#endif
