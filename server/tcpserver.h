#ifndef TCPSERVER_H
#define TCPSERVER_H

#include<QTcpServer>
#include<QFile>
#include<QTcpSocket>
#include<QTime>
#include<QString>
#include<QHostInfo>
#include<QDateTime>
#include<QProcess>
#include<QDataStream>
#include<QNetworkInterface>
#include<QStringList>
#include<QDebug>
#include<QTime>
#include <QObject>  // 添加QObject基类

class QTcpServer;
class QTcpSocket;
class TcpServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpServer(QWidget *parent = 0);
    ~TcpServer();
    void initServer();
    void refused();
signals:
   void sendFileName(QString );
private slots:
    void sendMessage();
    void updateClientProgress(qint64 );

private:

    QTcpServer *tcpServer;
    QTcpSocket *clientConnection;
    qint16 tcpPort;
    QFile *localFile ;

    qint64 payloadSize ;
    qint64 TotalBytes ;
    qint64 bytesWritten ;
    qint64 bytestoWrite;

    QString theFileName;
    QString fileName;

    QTime time;
    QByteArray outBlock;

};

#endif // TCPSERVER_H
