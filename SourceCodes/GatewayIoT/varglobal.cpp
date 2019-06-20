#include "varglobal.h"
#include <QSerialPort>
#include <QByteArray>

QSerialPort* serialZb = new QSerialPort;
QSerialPort* serialLr = new QSerialPort;

baudrate_type SerialBaud[9] = {{"115200", 115200}, {"57600", 57600}, {"38400", 38400}, {"19200", 19200}, \
                               {"14400", 14400}, {"9600", 9600}, {"4800", 4800}, {"2400", 24000}, {"1200", 1200}};
datasize_type SerialDataSize[4] = {{"8", 8}, {"7", 7}, {"6", 6}, {"5", 5}};

uint8 char_to_hex(char c)
{
    uint8 hex = (('0' <= c && c <= '9') ? (c - '0') : \
                (('A' <= c && c <= 'F') ? (c - 55) : \
                (('a' <= c && c <= 'f') ? (c - 87) : '0')));
    return hex;
}

uint8 _2char_to_hex(char c[2])
{
    uint8 hex;
    c[0] = char_to_hex(c[0]);
    c[1] = char_to_hex(c[1]);
    hex = c[1] + c[0]*16;
    return hex;
}

QByteArray string_to_hex(QByteArray string)
{
    QByteArray hex;
    int i = 0;
    int num_char = string.size();

    i = num_char - 1;
    if(num_char%2)
    {
        hex[i/2] = char_to_hex(string[i]);
        i -= 1;
    }
    while (i < 2)
    {
        string[i] = char_to_hex(string[i]);
        string[i - 1] = char_to_hex(string[i - 1]);
        hex[i/2] = string[i - 1] + string[i]*16;
        i -= 2;
    }

    return hex;
}
