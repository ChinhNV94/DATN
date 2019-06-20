#include "json.h"

void sendJson(QString add, QString status)
{
    QUrl url = QUrl("https://datn20182.000webhostapp.com/receiveJson.php");
    QNetworkAccessManager manager;
    QNetworkRequest request;
    QByteArray content = "";

    request.setUrl(url);
    content += "Adress=" + add.toUtf8() + "&";
    content += "Status=" + status.toUtf8();
    manager.post(request, content);
}
