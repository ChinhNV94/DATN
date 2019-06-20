#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>

extern "C" {
    #include "../MyZigbeeStack/zigbee.h"
    #include "../MyZigbeeStack/zigbee_zdo.h"
}

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void load_baudrate(void);
    void load_datasize(void);
    void load_command(void);
    void open_serialport(QSerialPort*);
    void serialport_write(QString*);
    void display_frame(frame_type fr);
    void createTable();

signals:
    void callZigbeeTask();

public slots:
    void load_serialport(void);
    void ZigbeeTask();
    void read_dataZb(void);

private slots:
    void on_pbt_OpenZb_clicked();

    void on_pbt_ClearZb_clicked();

    void on_pbt_SendCmdZb_clicked();

    void on_rbViewAll_clicked();

    void on_sbBeginSpots_editingFinished();

    void on_sbEndSpots_editingFinished();

    void on_btView_clicked();

    void on_btClear_clicked();

private:
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
