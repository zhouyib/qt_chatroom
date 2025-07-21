#ifndef SERVERHANDLER_H
#define SERVERHANDLER_H

#include "txtmsghandler.h"
#include "textmessage.h"
#include <QList>
#include <QMap>
#include <QTcpSocket>
#include <QTcpServer>
#include "tcpclient.h"
#include "tcpserver.h"
#include <QDebug>

class ServerHandler : public TxtMsgHandler
{

    typedef void(ServerHandler::*MSGHandler)(QTcpSocket&, TextMessage&);

    struct Node {
        QString id;
        QString pwd;
        QString ipaddr;
        QTcpSocket* socket;

    public:
        Node() : id(""),pwd(""),socket(nullptr) {

        }
    };

    QList<Node*> m_nodeList;
    QString rename;
    TcpServer *server;

    QString getOnlineUserId();
    void sendToAllOnlineUser(TextMessage& tm);
    void hasPendingFile(QString username,QString ipaddr);

public:
    ServerHandler();
    ~ServerHandler();
    void handle(QTcpSocket& obj, TextMessage& message);
public slots:
    void getFileName(QString);


};

#endif // SERVERHANDLER_H
