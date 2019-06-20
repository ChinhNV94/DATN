/*************************************************************************/
/* Include */

#include "serial.h"
#include <QByteArray>

extern "C" {
    #include "../MyZigbeeStack/zigbee.h"
}

/*************************************************************************/
/* Global variable */

QSerialPort* serialZb = new QSerialPort;
QSerialPort* serialLr = new QSerialPort;

baudrate_type SerialBaud[9] = {{"115200", 115200}, {"57600", 57600}, {"38400", 38400}, {"19200", 19200}, \
                               {"14400", 14400}, {"9600", 9600}, {"4800", 4800}, {"2400", 24000}, {"1200", 1200}};
datasize_type SerialDataSize[4] = {{"8", 8}, {"7", 7}, {"6", 6}, {"5", 5}};

/*************************************************************************/
/* Function */

void PortOpen(QSerialPort *port, QString port_name, int baudrate)
{
    port->setPortName(port_name);
    port->setBaudRate(baudrate);
    port->setDataBits(QSerialPort::Data8);
    port->setStopBits(QSerialPort::OneStop);
    port->setParity(QSerialPort::NoParity);
    port->setFlowControl(QSerialPort::NoFlowControl);
    while(!port->isOpen())
    {
        port->open(QIODevice::ReadWrite);
    }
}

void PortClose(QSerialPort *port)
{
    while(port->isOpen())
    {
        port->close();
    }
}

void PortReadData(QSerialPort *port)
{
    uint8 i = 0;
    QByteArray ar = port->readAll();
    while(i < ar.length())
    {
        queue_putByte(&RxQueue, (uint8)(ar[i]));
    }
}

void PortSendData(QSerialPort *port)
{
    QByteArray data;
    for(uint8 i = 0; i < TxQueue.data_size; i++)
    {
        data += queue_getByte(&TxQueue);
    }
    port->write(data);
}
