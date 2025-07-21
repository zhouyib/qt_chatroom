#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#define  IP "192.168.71.204"

#include <QMainWindow>
#include <QWidget>
#include <QDebug>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPlainTextEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTime>
#include <QLineEdit>
#include <QListWidget>
#include <QLabel>
#include <QGridLayout>
#include "clientdemo.h"
#include "serverdemo.h"
#include "txtmsghandler.h"
#include <QKeyEvent>
#include "tcpclient.h"
#include "tcpserver.h"
#include "loginer.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class MainWindow : public QMainWindow,public TxtMsgHandler
{
    Q_OBJECT
    QString usr;
    QString pwd;
    ClientDemo m_Client;
    ServerDemo m_Server;
    tcpserver *server;

    void initMsgGrpBx();
    void initInputBx();
    void initMember();
    void initUserList();
    QString getIP();

public:
    MainWindow(QWidget *parent = nullptr);
    void setUsername(QString username);
    void setPassword(QString password);
    void handle(QTcpSocket& obj, TextMessage& message);
    bool initConnect();
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    QPoint m_dragPosition;
    void hasPendingFile(QString userName, QString fileName);
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override ;
    void mouseMoveEvent(QMouseEvent *event) override ;
private slots:
    void on_sendBtn_clicked();
    void on_pushButton_exit_clicked();
    void on_pushButton_min_clicked();
    void on_pushButton_clear_clicked();
    void on_sendToolBtn_clicked();

    void getFileName(QString);
};
#endif // MAINWINDOW_H
