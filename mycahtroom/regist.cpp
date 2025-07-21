#include "regist.h"
#include "ui_regist.h"
regist::regist(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::regist)
{
    ui->setupUi(this);
}

regist::~regist()
{
    delete ui;
}

void regist::on_button_cancel_clicked()
{
    // 隐藏当前的注册界面
    this->hide();
    // 获取父窗口（即登录界面）并显示
    if (this->parentWidget()) {
        this->parentWidget()->show();
    }
    // 释放当前注册界面的内存
   this->deleteLater();
}

void regist::on_button_regist_clicked()
{
    QString re_user,re_password;
    re_user=ui->user->text();
    re_password=ui->password->text();
    if(db.insertUser(re_user,re_password)==1 && re_user!="" && re_password!="")
    {
        QMessageBox::information(NULL, "注册成功", "恭喜你，注册成功", QMessageBox::Yes , QMessageBox::Yes);
        on_button_cancel_clicked();
    }
    else {
        QMessageBox *box=new QMessageBox (this);
        box->setText("注册失败");
        box->setStandardButtons(QMessageBox::Ok);
        box->setModal(false);
        box->show();
    }
}
