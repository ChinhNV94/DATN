#ifndef QTXML_H
#define QTXML_H

#include <QtXml/QtXml>
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>

#include <QTableWidget>
#include <QMessageBox>


extern QString file_name;


QString SelecteFile();
bool fileExists(QString path_file);
void CreateXmlFile(QString path_file);
uint8_t ReadFile(QString path_file);
void UpdateDataToFile(QString address, QString status);
void ShowDataToTable(QString path_file, QTableWidget *table);


#endif // QTXML_H
