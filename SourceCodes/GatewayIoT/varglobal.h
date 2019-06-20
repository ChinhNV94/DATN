#ifndef VARGLOBAL_H
#define VARGLOBAL_H

#include <QtGlobal>
#include "../MyZigbeeStack/data_type.h"

QT_BEGIN_NAMESPACE
class QSerialPort;
QT_END_NAMESPACE

typedef struct _baudrate
{
    char* label;
    uint64 value;
} baudrate_type;

typedef struct _datasize
{
    char* label;
    uint8 value;
} datasize_type;

extern QSerialPort* serialZb;
extern QSerialPort* serialLr;
extern baudrate_type SerialBaud[9];
extern datasize_type SerialDataSize[4];

uint8 char_to_hex(char c);
uint8 _2char_to_hex(char c[2]);
QByteArray string_to_hex(QByteArray string);


#endif // VARGLOBAL_H
