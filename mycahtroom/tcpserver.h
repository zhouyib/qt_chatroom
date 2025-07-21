#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QDialog>
#include<QTcpServer>
#include<QFile>
#include<QTcpSocket>
#include<QTime>
#include<QFileDialog>
#include<QString>
#include<QCloseEvent>
#include<QWidget>
#include<QKeyEvent>
#include "ui_tcpserver.h"
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
#include<QApplication>
#include<QTime>
namespace Ui {
class tcpserver;
}
class QTcpServer;
class QTcpSocket;
class tcpserver : public QDialog
{
    Q_OBJECT
public:
    explicit tcpserver(QWidget *parent = 0);
    ~tcpserver();
    void initServer();
    void refused();
protected:
    void closeEvent(QCloseEvent *);
signals:
   void sendFileName(QString );
private slots:
    void on_serverOpenBtn_clicked();
    void on_serverSendBtn_clicked();
    void on_serverCloseBtn_clicked();

    void sendMessage();
    void updateClientProgress(qint64 );
private:
    Ui::tcpserver *ui;

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
