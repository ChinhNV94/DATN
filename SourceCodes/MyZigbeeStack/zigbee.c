/*************************************************************************/
/* Include */

#include "zigbee.h"
#include "zigbee_zdo.h"
#include <stdlib.h>
#include <string.h>

/*************************************************************************/
/* Global variable */

/*uint8 RxBuffer[MAX_SIZE_RX_BUFFER];
uint8 TxBuffer[MAX_SIZE_TX_BUFFER];
uint16 RxQueue.data_size = 0;
uint16 tx_buffer_size = 0;
uint16 RxQueue.get_index = 0;
uint16 tx_buffer_read_index = 0;
uint16 rx_buffer_write_index = 0;
uint16 tx_buffer_write_index = 0;*/

uint8 zigbee_task = NONE;
queue RxQueue;
queue TxQueue;
uint8 rx_address_count = 0;
uint16_type rx_address[100];
frame_type rx_frame;
frame_type tx_frame;

data_16bit_label frame_control[4] = \
{ \
    {"Beacon frame", {0x00, 0x00}}, \
    {"Data frame", {0x20, 0x00}}, \
    {"Ack frame" , {0x40, 0x00}}, \
    {"Command frame" , {0x50, 0x00}} \
};

char* CMD[3] = {"rqData", "rqAck", "Beacon"};
char* Mode[3] = {"Unicast", "Multicast", "Broadcast"};

uint8 Blank_Spots = 100;

/*************************************************************************/
/* Function */

/* uint8 cs_calculation(frame_type frame)
{
    uint8 reVal = 0;
    uint8 i = 0;
    while(i < frame.header.data_length)
    {
        reVal += frame.payload.data[i];
        i++;
    }
    reVal ^= 0xFF;
    return reVal;
} */

void get_Rxheader()
{
    uint8 i = 0;
    /* rx_frame.header.start = queue_getByte(&RxQueue); */
    rx_frame.header.data_length = queue_getByte(&RxQueue);
    rx_frame.header.frame_ctrl.msb = queue_getByte(&RxQueue);
    rx_frame.header.frame_ctrl.lsb = queue_getByte(&RxQueue);
    rx_frame.header.dest_address.msb = queue_getByte(&RxQueue);
    rx_frame.header.dest_address.lsb = queue_getByte(&RxQueue);
    rx_frame.header.source_address.msb = queue_getByte(&RxQueue);
    rx_frame.header.source_address.lsb = queue_getByte(&RxQueue);

    while(i < rx_address_count)
    {
        if(rx_address[i].msb == rx_frame.header.source_address.msb \
        && rx_address[i].lsb == rx_frame.header.source_address.lsb)
        {
            break;
        }
        i++;
    }
    if(i == rx_address_count)
    {
        rx_address[rx_address_count] = rx_frame.header.source_address;
        rx_address_count++;
    }
}

data_16bit_label check_frame_ctrl()
{
    data_16bit_label f_ctrl;
    char label[30] = "";
    f_ctrl.label = "ERROR";
    f_ctrl.val.lsb = rx_frame.header.frame_ctrl.lsb;
    f_ctrl.val.msb = rx_frame.header.frame_ctrl.msb;

    switch(f_ctrl.val.msb)
    {
    case UNICAST:
        //f_ctrl.label = "UNICAST";
        strcat(label, "UNICAST");
        break;
    case MULTICAST:
        //f_ctrl.label = "MULTICAST";
        strcat(label, "MULTICAST");
        break;
    case BROADCAST:
        //f_ctrl.label = "BROADCAST";
        strcat(label, "BROADCAST");
        break;
    default:
        break;
    }

    switch(f_ctrl.val.lsb)
    {
    case COMMAND:
        strcat(label, " COMMAND");
        break;
    case ACK:
        strcat(label, " ACK");
        break;
    case BEACON:
        strcat(label, " BEACON");
        break;
    case DATA:
        strcat(label, " DATA");
        break;
    default:
        break;
    }
    f_ctrl.label = label;
    return f_ctrl;
}

char* get_CMD(frame_type fr)
{
    char* cmd;
    switch(fr.header.frame_ctrl.lsb)
    {
    case COMMAND:
      cmd = " COMMAND";
        break;
    case ACK:
      cmd = " ACK";
        break;
    case BEACON:
      cmd = " BEACON";
        break;
    case DATA:
      cmd = " DATA";
        break;
    case REQUEST_ACK:
        cmd = "REQUEST_ACK";
        break;
    case REQUEST_DATA:
        cmd = "REQUEST_DATA";
        break;
    default:
        cmd = null;
        break;
    }
    return cmd;
}

char* get_MODE(frame_type fr)
{
    char* mode;
    switch(fr.header.frame_ctrl.msb)
    {
    case UNICAST:
        mode = "UNICAST";
        break;
    case MULTICAST:
        mode = "MULTICAST";
        break;
    case BROADCAST:
        mode = "BROADCAST";
        break;
    default:
        mode = null;
        break;
    }
    return mode;
}

void get_Rxpayload()
{
    uint8 i = 0;
    uint8 length = rx_frame.header.data_length;
    if(length > 0)
    {
        rx_frame.payload.data = (uint8 *) malloc(length);
        while(i < length)
        {
            rx_frame.payload.data[i] = queue_getByte(&RxQueue);
            i++;
        }
        /* rx_frame.payload.cs = queue_getByte(&RxQueue);
        rx_frame.payload.end = queue_getByte(&RxQueue); */
    }
    else
    {
        rx_frame.payload.data = NULL;
        /* rx_frame.payload.cs = queue_getByte(&RxQueue);
        rx_frame.payload.end = queue_getByte(&RxQueue); */
    }
}

void delete_Rxframe()
{
    RxQueue.data_size = 0;
    /* rx_frame.header.data_length = 0;
    rx_frame.header.frame_ctrl = 0;
    rx_frame.header.dest_address = 0;
    rx_frame.header.source_address = 0; */
    rx_frame.payload.data = NULL;
    RxQueue.get_index += rx_frame.header.data_length;
}

uint8 get_Rxframe()
{
    uint8 reval = 0;
    get_Rxheader();
    if(rx_frame.header.dest_address.msb == add_sensor[ZIGBEE_NODE].msb \
    && rx_frame.header.dest_address.lsb == add_sensor[ZIGBEE_NODE].lsb)
    {
        get_Rxpayload();
    }
    else
    {
        delete_Rxframe();
        reval = 1;
    }
    return reval;
}

void setTxHeader(uint8 len, uint8 mode, uint8 cmd, uint16_type dest_add, uint16_type source_add)
{
    /* tx_frame.header.start = 0x01; */
    tx_frame.header.data_length = len;
    tx_frame.header.frame_ctrl.msb =mode;
    tx_frame.header.frame_ctrl.lsb =cmd;
    tx_frame.header.dest_address.msb = dest_add.msb;
    tx_frame.header.dest_address.lsb = dest_add.lsb;
    /* tx_frame.header.source_address = add_sensor[ZIGBEE_NODE]; */
    tx_frame.header.source_address.msb = source_add.msb;
    tx_frame.header.source_address.lsb = source_add.lsb;
}

void set_Txheader(uint8 len, uint16_type ctrl, uint16_type dest_add, uint16_type source_add)
{
    /* tx_frame.header.start = 0x01; */
    tx_frame.header.data_length = len;
    tx_frame.header.frame_ctrl.msb =ctrl.msb;
    tx_frame.header.frame_ctrl.lsb =ctrl.lsb;
    tx_frame.header.dest_address.msb = dest_add.msb;
    tx_frame.header.dest_address.lsb = dest_add.lsb;
    /* tx_frame.header.source_address = add_sensor[ZIGBEE_NODE]; */
    tx_frame.header.source_address.msb = source_add.msb;
    tx_frame.header.source_address.lsb = source_add.lsb;
}

void set_Txpayload(uint8 data[])
{
    uint8 i = 0;
    if(data != null)
    {
        tx_frame.payload.data = (uint8 *) malloc(tx_frame.header.data_length);
        while(i < tx_frame.header.data_length)
        {
            tx_frame.payload.data[i] = data[i];
            i++;
        }
        /* tx_frame.payload.cs = cs_calculation(tx_frame);
        tx_frame.payload.end = 0x04; */
    }
}

void set_Txframe(uint8 len, uint16_type ctrl, uint16_type dest_add, uint16_type source_add, uint8 data[])
{
    set_Txheader(len, ctrl, dest_add, source_add);
    set_Txpayload(data);
}

void setTxFrame(uint8 len, uint8 mode, uint8 cmd, uint16_type dest_add, uint16_type source_add, uint8 data[])
{
    setTxHeader(len, mode, cmd, dest_add, source_add);
    set_Txpayload(data);
}


void putTxFrameToTxBuffer()
{
    uint8 i = 0;
    /* queue_putByte(TxQueue, tx_frame.header.start); */
    queue_putByte(&TxQueue, tx_frame.header.data_length);
    queue_putByte(&TxQueue, (uint8)(tx_frame.header.frame_ctrl.msb));
    queue_putByte(&TxQueue, (uint8)(tx_frame.header.frame_ctrl.lsb));
    queue_putByte(&TxQueue, (uint8)(tx_frame.header.dest_address.msb));
    queue_putByte(&TxQueue, (uint8)(tx_frame.header.dest_address.lsb));
    queue_putByte(&TxQueue, (uint8)(tx_frame.header.source_address.msb));
    queue_putByte(&TxQueue, (uint8)(tx_frame.header.source_address.lsb));

    for(i = 0; i < tx_frame.header.data_length; i++)
    {
        queue_putByte(&TxQueue, tx_frame.payload.data[i]);
    }
    /* queue_putByte(&TxQueue, tx_frame.payload.cs);
     queue_putByte(&TxQueue, tx_frame.payload.end); */
}

void queue_putFrame(queue *q, frame_type fr)
{
    uint8 i = 0;
    /* queue_putByte(q, tx_frame.header.start); */
    queue_putByte(q, fr.header.data_length);
    queue_putByte(q, (uint8)(fr.header.frame_ctrl.msb));
    queue_putByte(q, (uint8)(fr.header.frame_ctrl.lsb));
    queue_putByte(q, (uint8)(fr.header.dest_address.msb));
    queue_putByte(q, (uint8)(fr.header.dest_address.lsb));
    queue_putByte(q, (uint8)(fr.header.source_address.msb));
    queue_putByte(q, (uint8)(fr.header.source_address.lsb));

    for(i = 0; i < fr.header.data_length; i++)
    {
        queue_putByte(q, fr.payload.data[i]);
    }
    /* queue_putByte(q, fr.payload.cs);
     queue_putByte(q, fr.payload.end); */
}

void zigbeeTask()
{

}

