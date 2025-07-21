#include "mainwindow.h"

#include <QApplication>
#include "regist.h"
#include "loginer.h"
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    loginer lo;
    lo.setWindowTitle("聊天室登录界面");
    //添加最小化按钮
    Qt::WindowFlags windowFlag  = Qt::Dialog;
    windowFlag                  |= Qt::WindowMinimizeButtonHint;
    windowFlag                  |= Qt::WindowCloseButtonHint;
    windowFlag                  |= Qt::FramelessWindowHint;
    lo.setWindowFlags(windowFlag);
    lo.setWindowIcon(QIcon(":/res/login.ico"));
    lo.show();

    return a.exec();
}
