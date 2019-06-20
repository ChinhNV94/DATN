#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "serial.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QIODevice>
#include <QThread>
#include <QTimer>
#include <QDebug>
#include "qtxml.h"
#include "json.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    load_baudrate();
    load_datasize();
    load_command();

    queue_empty(&RxQueue);
    queue_empty(&TxQueue);

    /* setup window strart */
    ui->pte_DisplayDataZb->setReadOnly(true);
    ui->cb_DatasizeZb->setEnabled(false);
    ui->ck_ParityZb->setEnabled(false);
    ui->leTotalSpots->setReadOnly(true);
    ui->leBlankSpots->setReadOnly(true);

    /* QTableWidget seting */
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->setColumnWidth(0,ui->tableWidget->width()/3);
    ui->tableWidget->setColumnWidth(1,ui->tableWidget->width()/3);
    ui->rbViewAll->setChecked(true);
    ui->sbBeginSpots->setValue(0);
    ui->sbEndSpots->setValue(99);
    ui->leTotalSpots->setText("100");
    createTable();

    /* connect signal and slot */
    QTimer *timer = new QTimer();
    timer->start(1000);
    connect(timer, SIGNAL(timeout()),this,SLOT(load_serialport()));
    connect(serialZb,SIGNAL(readyRead()),this,SLOT(read_dataZb()));
    connect(this, SIGNAL(callZigbeeTask()),this,SLOT(ZigbeeTask()));

    CreateXmlFile(file_name);
    ui->leBlankSpots->setText(QString::number(ReadFile(file_name)));
}

MainWindow::~MainWindow()
{
    while(serialZb->isOpen())
    {
        serialZb->close();
    }
    while(serialLr->isOpen())
    {
        serialLr->close();
    }

    delete ui;
}

void MainWindow::load_serialport()
{
    Q_FOREACH(QSerialPortInfo port, QSerialPortInfo::availablePorts())
    {
        if(!port.isBusy())
        {
            if(ui->cb_DeviceZb->findText(port.portName()) < 0)
            {
                ui->cb_DeviceZb->addItem(port.portName());
            }
        }
    }
}

void MainWindow::load_baudrate()
{
    unsigned char i = 0;
    while(i < 9)
    {
        ui->cb_BaudrateZb->addItem(SerialBaud[i].label, SerialBaud[i].value);
        i++;
    }
    ui->cb_BaudrateZb->setCurrentIndex(5);
}

void MainWindow::load_datasize()
{
    unsigned char i = 0;
    while(i < 4)
    {
        ui->cb_DatasizeZb->addItem(SerialDataSize[i].label, SerialDataSize[i].value);
        i++;
    }
    ui->cb_DatasizeZb->setCurrentIndex(0);
}

void MainWindow::load_command()
{
    unsigned char i = 0;
    while(i < 3)
    {
        ui->cb_CmdZb->addItem(CMD[i]);
        ui->cb_ModeZb->addItem(Mode[i]);
        i++;
    }
}

void MainWindow::open_serialport(QSerialPort* serial)
{
    /* serial->open(QIODevice::ReadWrite);
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setParity(QSerialPort::NoParity);
    serial->setFlowControl(QSerialPort::NoFlowControl); */

    serial->open(QIODevice::ReadWrite);
    if(serial == serialZb)
    {
        serial->setBaudRate(ui->cb_BaudrateZb->currentData().toInt());
    }
    serial->setDataBits(QSerialPort::Data8);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setParity(QSerialPort::NoParity);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    while(!serial->isOpen()) serial->open(QIODevice::ReadWrite);
}

void MainWindow::on_pbt_OpenZb_clicked()
{
    if(ui->pbt_OpenZb->text() == "Open")
    {
        serialZb->setPortName(ui->cb_DeviceZb->currentText());
        open_serialport(serialZb);

        if(serialZb->isOpen())
        {
            ui->pbt_OpenZb->setText("Close");
            ui->pte_DisplayDataZb->insertPlainText("\n" + serialZb->portName() + " is opened!!!");
            ui->cb_DeviceZb->setEnabled(false);
            ui->cb_BaudrateZb->setEnabled(false);
        }
        else
        {
            ui->pte_DisplayDataZb->insertPlainText("\nCan not open serialport!!!");
        }
    }
    else
    {
        serialZb->close();
        ui->pbt_OpenZb->setText("Open");
        ui->pte_DisplayDataZb->insertPlainText("\n" + ui->cb_DeviceZb->currentText() + " is closed!!!");
        ui->cb_DeviceZb->setEnabled(true);
        ui->cb_BaudrateZb->setEnabled(true);
    }
}

void MainWindow::on_pbt_ClearZb_clicked()
{
    ui->pte_DisplayDataZb->setPlainText("");
}

void MainWindow::read_dataZb()
{
    uint8 i = 0;
    QByteArray arr = serialZb->readAll();

    while(i < arr.length())
    {
        queue_putByte(&RxQueue, (uint8)(arr[i]));
        i++;
    }

    if(RxQueue.data_size >= RxQueue.data[RxQueue.get_index] + HEADER_LENGTH)
    {
        zigbee_task = ONRECEIVE;
        emit callZigbeeTask();
    }
}

void MainWindow::display_frame(frame_type fr)
{
    uint8 i = 1;
    QByteArray str;

    //ui->pte_DisplayDataZb->insertPlainText("\n");
    if(get_MODE(fr) == null || get_CMD(fr) == null)
    {
        ui->pte_DisplayDataZb->insertPlainText("Invalid frame");
    }
    else
    {
        //ui->pte_DisplayDataZb->insertPlainText(get_MODE(fr));
        //ui->pte_DisplayDataZb->insertPlainText(get_CMD(fr));
        //ui->pte_DisplayDataZb->insertPlainText(" :");
        if(fr.payload.data != NULL)
        {
            if('F' == fr.payload.data[0])
            {
                str += "Filled";
            }
            else if('E' == fr.payload.data[0])
            {
                str += "Blank";
            }
            else
            {
                str += "Data error!";
            }
            while(i < fr.header.data_length)
            {
                str += fr.payload.data[i];
                i++;
            }
            /* ui->pte_DisplayDataZb->insertPlainText(str.toHex()); */
            ui->pte_DisplayDataZb->insertPlainText(str);
         }
    }
}

void MainWindow::ZigbeeTask()
{
   if(ONRECEIVE == zigbee_task)
   {
        QByteArray add;
        uint8 t = rx_address_count;

        if(!get_Rxframe())
        {
            QByteArray arr;
            QString stt;

            arr += rx_frame.header.source_address.msb;
            arr += rx_frame.header.source_address.lsb;
            ui->pte_DisplayDataZb->insertPlainText("\nReceived from node:" + arr.toHex() + "\nStatus:");
            display_frame(rx_frame);
            if(rx_frame.payload.data[0] == 'F')
            {
                UpdateDataToFile(arr.toHex(), "FilLed");
                stt += "Filled";
            }
            if(rx_frame.payload.data[0] == 'E')
            {
                UpdateDataToFile(arr.toHex(), "Blank");
                stt += "Blank";
            }
            ui->leBlankSpots->setText(QString::number(ReadFile(file_name)));
            sendJson(QString::fromStdString(arr.toStdString()), stt);
        }
        if(t < rx_address_count)
        {
            add += rx_address[rx_address_count - 1].msb;
            add += rx_address[rx_address_count - 1].lsb;
            ui->cb_NodeZb->addItem(add.toHex());
        }

        if(RxQueue.data_size >= RxQueue.data[RxQueue.get_index] + HEADER_LENGTH)
        {
            zigbee_task = ONRECEIVE;
            emit callZigbeeTask();
        }
        else
        {
            zigbee_task = NONE;
        }
    }
    else if(ONTRANSMIT == zigbee_task)
    {
        QByteArray arr;
        qDebug() << "ONTRANSMIT";

        putTxFrameToTxBuffer();
        while(TxQueue.data_size)
        {
            arr += queue_getByte(&TxQueue);
        }
        serialZb->write(arr);

        zigbee_task = NONE;
    }
    else
    {
        zigbee_task = NONE;
    }

}

void MainWindow::on_pbt_SendCmdZb_clicked()
{
    uint8 mode, cmd, add, i;
    QByteArray arr;
    uint16_type dest_add;

    mode = ui->cb_ModeZb->currentIndex();
    switch(ui->cb_CmdZb->currentIndex())
    {
    case 0:
        cmd = REQUEST_DATA;
        break;
    case 1:
        cmd = REQUEST_ACK;
        break;
    case 2:
        cmd = BEACON;
        break;
    default:
        cmd = null;
        break;
    }
    if(BROADCAST == mode)
    {
        dest_add = add_sensor[ZIGBEE_NODE];
    }
    else
    {
        add = ui->cb_NodeZb->currentText().toInt();
        for(i = 0; i < rx_address_count; i++)
        {
            if(rx_address[i].lsb == add)
            {
                break;
            }
        }
        if(i < rx_address_count)
        {
            dest_add = rx_address[i];
        }
        else
        {
            ui->pte_DisplayDataZb->insertPlainText("\nTx Frame ERROR!!!");
            return;
        }
    }

    setTxFrame(0, mode, cmd, dest_add, add_sensor[ZIGBEE_NODE], null);
    arr += rx_address[i].msb;
    arr += rx_address[i].lsb;
    ui->pte_DisplayDataZb->insertPlainText("\nSend request to:" + arr.toHex());
    ui->pte_DisplayDataZb->insertPlainText("\n");
    ui->pte_DisplayDataZb->insertPlainText(get_MODE(tx_frame));
    ui->pte_DisplayDataZb->insertPlainText(" ");
    ui->pte_DisplayDataZb->insertPlainText(get_CMD(tx_frame));
    display_frame(tx_frame);

    qDebug() << "callZigbeeTask";
    zigbee_task = ONTRANSMIT;
    emit callZigbeeTask();
}

void MainWindow::on_btClear_clicked()
{
    ui->pte_DisplayDataZb->setPlainText("");
}

void MainWindow::createTable()
{
    for(uint8_t i = 0; i < 100; i++)
    {
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    }
}

void MainWindow::on_rbViewAll_clicked()
{

}

void MainWindow::on_sbBeginSpots_editingFinished()
{

}

void MainWindow::on_sbEndSpots_editingFinished()
{

}

void MainWindow::on_btView_clicked()
{
    ShowDataToTable(file_name, ui->tableWidget);
}

