#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QDialog>
#include<QTcpServer>
#include<QFile>
#include<QTcpSocket>
#include<QTime>
#include<QFileDialog>
#include<QString>
#include<QCloseEvent>
#include<QHostAddress>
#include "ui_tcpclient.h"
#include<QHostInfo>
#include<QMessageBox>
#include<QDateTime>
#include<QProcess>
#include<QDataStream>
#include<QScrollBar>
#include<QFont>
#include<QNetworkInterface>
#include<QStringList>
#include<QDebug>

namespace Ui {
class tcpclient;
}

class tcpclient : public QDialog
{
    Q_OBJECT

public:
    explicit tcpclient(QWidget *parent = 0);
    ~tcpclient();


    void setFileName(QString fileName);
    void setHostAddress(QHostAddress address);

    void closeEvent(QCloseEvent *);

     QFile *localFile;

private slots:
    void on_tcpClientCancleBtn_clicked();
    void on_tcpClientCloseBtn_clicked();

    void readMessage();
    void displayError(QAbstractSocket::SocketError);
    void newConnect();


private:
    Ui::tcpclient *ui;

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
