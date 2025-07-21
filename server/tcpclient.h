#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include<QTcpServer>
#include<QFile>
#include<QTcpSocket>
#include<QTime>
#include<QString>
#include<QHostAddress>
#include<QHostInfo>
#include<QDateTime>
#include<QProcess>
#include<QDataStream>
#include<QNetworkInterface>
#include<QStringList>
#include<QDebug>
#include <QObject>  // 添加QObject基类

class TcpClient : public QObject
{
    Q_OBJECT

public:
    explicit TcpClient(QWidget *parent = 0);
    ~TcpClient();


    void setFileName(QString fileName);
    void setHostAddress(QHostAddress address);
     QFile *localFile;

private slots:
    void readMessage();
    void displayError(QAbstractSocket::SocketError);
    void newConnect();

private:
    QTcpSocket *tcpClient;
    qint16  tcpPort ;

    QHostAddress hostAddress;

    qint64 TotalBytes ;
    qint64 bytesReceived;
    qint64 fileNameSize ;
    qint64 blockSize;
    QString fileName;

    QTime time;
    QByteArray inBlock;

};

#endif // TCPCLIENT_H
