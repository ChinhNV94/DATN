#include "myZigbee.h"
#include "myZigbeeZdo.h"
#include "myQueue.h"
#include <stdlib.h>
#include <string.h>

/* Global variable */
uint8 rx_address_count = 0;
uint16_type rx_address[100];
frame_type rx_frame;
frame_type tx_frame;
uint8* tx_data;

uint8 cs_calculation(frame_type frame)
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
}

void get_RxHeader()
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

frame_ctrl_type check_frame_ctrl()
{
    frame_ctrl_type f_ctrl;
    f_ctrl.label = "ERROR";
    f_ctrl.val.lsb = rx_frame.header.frame_ctrl.lsb;
    f_ctrl.val.msb = rx_frame.header.frame_ctrl.msb;

    switch(f_ctrl.val.msb)
    {
    case 0x01:
        f_ctrl.label = "UNICAST";
        break;
    case 0x02:
        f_ctrl.label = "MULTICAST";
        break;
    case 0xFF:
        f_ctrl.label = "BROADCAST";
        break;
    default:
        break;
    }

    switch(f_ctrl.val.lsb)
    {
    case 0x00:
        strcat(f_ctrl.label, " BEACON");
        break;
    case 0x01:
        strcat(f_ctrl.label, " DATA");
        break;
    case 0x02:
        strcat(f_ctrl.label, " ACK");
        break;
    case 0x03:
        strcat(f_ctrl.label, " COMMAND");
        break;
    default:
        break;
    }
    return f_ctrl;
}

void get_RxPayload()
{
    uint8 i = 0;
    uint8 length = rx_frame.header.data_length;
    if(length > 0)
    {
        rx_frame.payload.data = (uint8*) malloc(length);
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
    /* rx_frame.header.data_length = 0;
    rx_frame.header.frame_ctrl = 0;
    rx_frame.header.dest_address = 0;
    rx_frame.header.source_address = 0;
    rx_frame.payload.data = NULL; */
    /*rx_buffer_size = 0*/;
    rx_buffer_size -= rx_frame.header.data_length;
    rx_buffer_read_index += rx_frame.header.data_length;
}

uint8 get_Rxframe()
{
    uint8 reval = 0;
    get_RxHeader();
    if(rx_frame.header.dest_address.msb == add_sensor[ZIGBEE_NODE].msb \
    && rx_frame.header.dest_address.lsb == add_sensor[ZIGBEE_NODE].lsb)
    {
        get_RxPayload();
    }
    else
    {
        delete_Rxframe();
        reval = 1;
    }
    return reval;
}

void set_TxHeader(uint8 len, uint16_type ctrl, uint16_type dest_add, uint16_type source_add)
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

void set_TxPayload(uint8 data[])
{
    uint8 i = 0;
    tx_frame.payload.data = (uint8 *) malloc(tx_frame.header.data_length);
    while(i < tx_frame.header.data_length)
    {
        tx_frame.payload.data[i] = data[i];
        i++;
    }
    /* tx_frame.payload.cs = cs_calculation(tx_frame);
    tx_frame.payload.end = 0x04; */
}

void set_TxFrame(uint8 len, uint16_type ctrl, uint16_type dest_add, uint16_type source_add, uint8 data[])
{
    set_TxHeader(len, ctrl, dest_add, source_add);
    set_TxPayload(data);
}

void putTxFrameToTxBuffer()
{
    uint8 i = 0;
    /* queue_putByte(&TxQueue, tx_frame.header.start); */
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

void zigbeeTask()
{

}

