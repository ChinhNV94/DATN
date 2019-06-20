#ifndef VARGLOBAL_H
#define VARGLOBAL_H

/*************************************************************************/
/* Include */

#include <QSerialPort>
#include <QString>
#include "../MyZigbeeStack/data_type.h"

/*************************************************************************/
/* Define & Typedef */

typedef struct _baudrate
{
    QString label;
    uint64 value;
} baudrate_type;

typedef struct _datasize
{
    QString label;
    uint8 value;
} datasize_type;

/*************************************************************************/
/* Global variable */

extern QSerialPort* serialZb;
extern QSerialPort* serialLr;
extern baudrate_type SerialBaud[9];
extern datasize_type SerialDataSize[4];

/*************************************************************************/
/* Function */

void PortOpen(QSerialPort *port, QString port_name, int baudrate);
void PortClose(QSerialPort *port);
void PortReadData(QSerialPort *port);
void PortSendData(QSerialPort *port);

#endif // VARGLOBAL_H
