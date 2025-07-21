#ifndef LOGINER_H
#define LOGINER_H

#include <QDebug>
#include <QDialog>
#include "regist.h"
#include "database.h"
#include "mainwindow.h"
#include <QSettings>
#include <QTextCodec>
#include <QCompleter>
namespace Ui {
class loginer;
}

class loginer : public QDialog
{
    Q_OBJECT

public:
    explicit loginer(QDialog *parent = nullptr);
    ~loginer();
    QString getUser();
    QString getPwd();

private slots:
    void on_button_regist_clicked();
    void on_button_login_clicked();
    void on_user_currentIndexChanged(const QString &arg1);

    void on_pushButton_exit_clicked();
    void on_pushButton_min_clicked();

protected:
    void mousePressEvent(QMouseEvent *event)  ;
    void mouseMoveEvent(QMouseEvent *event) ;
private:
    Ui::loginer *ui;
    QString m_User;
    QString m_Pwd;
    DataBaseDemo db;
    QSettings *settings;
    QStringList m_usernameList;

    QPoint m_dragPosition;    //当前鼠标位置

    bool autologin;

    void saveAccountInfo(const QString &user, const QString &pwd);
    void updateUsernameList();
    void gif_show();
    void save_passwd();
    void auto_login();
};

#endif // LOGINER_H
