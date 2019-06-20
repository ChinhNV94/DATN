#include "qtxml.h"
#include <QDebug>

QString file_name = "./GatewayIoT/dataSpots.xml";
QString doc = "dataSpots";


/* QString SelecteFile()
{
    // Call the file selection dialog to save
    QString filename = QFileDialog::getSaveFileName(this,
                                           tr("Save Xml"), ".",
                                           tr("Xml files (*.xml)"));
    return filename;
}*/

bool fileExists(QString path_file)
{
    QFileInfo fileInfo(path_file);
    // check if path exists and if yes: Is it really a file and no directory?
    return fileInfo.exists() && fileInfo.isFile();
}

void CreateXmlFile(QString path_file)
{
    if(!fileExists(path_file))
    {
        /* Open the file for writing using the path specified in lineEditWrite */
        QFile file(path_file);
        file.open(QIODevice::WriteOnly);

        /* Create an object, through which the recording to file */
        QXmlStreamWriter xmlWriter(&file);
        xmlWriter.setAutoFormatting(true);  // Set the auto-formatting text
        xmlWriter.writeStartDocument();     // run record in a document

        /* Start tag "root" */
        xmlWriter.writeStartElement("root");   // Write the first element of his name

        for(uint8_t i = 0; i < 100; i++)
        {
            /* Start tag "Node" */
            xmlWriter.writeStartElement("Node");
            QString add;
            if(i < 10)
            {
                add = "000" + QString::number(i);
            }
            else
            {
                add = "00" + QString::number(i);
            }
            //xmlWriter.writeAttribute("ID", QString::number(i));
            xmlWriter.writeAttribute("Address", add);
            xmlWriter.writeAttribute("Status", "Blank");
            /* End tag "Node" */
            xmlWriter.writeEndElement();
        }

        /* Close tag "root" */
        xmlWriter.writeEndElement();
        /* Finish the writing in the document */
        xmlWriter.writeEndDocument();
        file.close();
    }
}

uint8_t ReadFile(QString path_file)
{
    uint8_t count_blank = 0;

    QFile file;
    file.setFileName(file_name);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QDomDocument document(doc);
    document.setContent(&file);
    file.close();

    //do some reading here
    QDomNodeList list = document.elementsByTagName("Node");

    for(int i = 0; i < list.count(); i++)
    {
        QDomNode itemnode = list.at(i);

        //convert to element
        if(itemnode.isElement())
        {
            QDomElement itemele = itemnode.toElement();
            if("Blank" == itemele.attribute("Status"))
            {
                count_blank++;
            }
        }
    }
    return count_blank;
}

void UpdateDataToFile(QString address, QString status)
{
    QFile file;
    file.setFileName(file_name);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QDomDocument document(doc);
    document.setContent(&file);
    file.close();

    //do some reading here
    QDomNodeList list = document.elementsByTagName("Node");

    for(int i = 0; i < list.count(); i++)
    {
        QDomNode itemnode = list.at(i);

        //convert to element
        if(itemnode.isElement())
        {
            QDomElement itemele = itemnode.toElement();
            if(address == itemele.attribute("Address"))
            {
                if(status != itemele.attribute("Status"))
                {
                    itemele.setAttribute("Status", status);
                }
            }
        }
    }

    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream stream(&file);
    //document.save(stream, 4);
    stream << document.toString();
    file.close();
}

void ShowDataToTable(QString path_file, QTableWidget *table)
{
    uint8_t row = 0;

    /* Open the file for reading */
    QFile file(path_file);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox box;
        box.setText("Failed to open file!!!");
        box.exec();
    }
    else
    {
        /* Create an object, through which the reading of the file */
        QXmlStreamReader xmlReader;
        xmlReader.setDevice(&file);
        xmlReader.readNext();   // Moves to the first item in the file

        /* Work in a loop as long as we reach the end of the document */
        while(!xmlReader.atEnd())
        {
            /* Check whether the beginning of the tag element */
            if(xmlReader.isStartElement())
            {
                /* Check whether the tag refers to one of the root.
                 * If "YES", then a check attribute and records
                 * */
                if(xmlReader.name() == "Node")
                {
                    /* Taking all of the tag attributes,
                     * and through them to check for compliance with the required attribute to us
                     * */
                    foreach(const QXmlStreamAttribute &attr, xmlReader.attributes())
                    {
                        /* If the attribute is found, get value
                         * */
                        if (attr.name().toString() == "Address")
                        {
                            QString attribute_value = attr.value().toString();
                            table->setItem(row, 0, new QTableWidgetItem(attribute_value));
                        }
                        if (attr.name().toString() == "Status")
                        {
                            QString attribute_value = attr.value().toString();
                            table->setItem(row, 1, new QTableWidgetItem(attribute_value));
                        }
                    }
                    row++;
                }
            }
            xmlReader.readNext(); // Go to the next file element
        }
        file.close(); // Close the file

        /* This code is not checking for the closing tag because it is not necessary,
         * but it allows the functional QXmlStreamReader
         * */
    }
}
