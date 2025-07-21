#include "loginer.h"
#include "ui_loginer.h"
#include <QMovie>

loginer::loginer(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::loginer)
{
    ui->setupUi(this);
    ui->button_regist->setStyleSheet("border: none;");
    ui->password->setEchoMode(QLineEdit::Password);
    ui->label_head->raise();//显示在最上层
    ui->user->setView(new QListView());
    //禁止调节窗体大小
    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint);
    setFixedSize(this->width(), this->height());
    //读取qss样式文件
    QString qss;
    QFile qssFile(":/interface/interface_log.qss");
    qssFile.open(QFile::ReadOnly);
    if(qssFile.isOpen())
    {
        qss=QLatin1String(qssFile.readAll());
        qDebug()<<qss;
        this->setStyleSheet(qss);
        qssFile.close();
    }
    gif_show();
    save_passwd();
    ui->password->setStyleSheet("background-image: url();");

    auto_login();
    updateUsernameList();
}

loginer::~loginer()
{
    delete ui;
}
//自动登录检测
void loginer::auto_login(){
    if(autologin){
        ui->password->setStyleSheet("background-image: url();");
    }
    else {
        ui->password->setVisible(true);

    }

}
//创建用来保存密码的配置文件
void loginer::save_passwd(){
    QString exePath=QCoreApplication::applicationDirPath();
    //配置文件为可执行程序所在的目录下的local.ini
    QString iniFile=exePath+"/local.ini";
    settings = new QSettings(iniFile, QSettings::IniFormat,this);
    settings->setIniCodec(QTextCodec::codecForName("UTF-8"));
}
//显示动图
void loginer::gif_show(){
    QMovie *movie = new QMovie(":/res/label_gif.gif");
    ui->label_gif->setMovie(movie); // 1. 设置要显示的 GIF 动画图片
    movie->start();         // 2. 启动动画
    ui->label_gif->show();
}


// 获取用户名
QString loginer::getUser()
{
    return m_User;
}

// 获取密码
QString loginer::getPwd()
{
    return m_Pwd;
}

// 保存账号密码
void loginer::saveAccountInfo(const QString &user, const QString &pwd)
{
    settings->setValue("accounts/" + user, pwd);
    updateUsernameList();
}

// 更新用户名列表
void loginer::updateUsernameList()
{
    ui->user->setEditable(true);
    ui->user->clear();
    m_usernameList.clear();
    QStringList keys = settings->allKeys();
    for(int i=0;i<keys.size();i++){
        keys[i].replace("accounts/","");
    }
    for (const QString &key : keys) {
        m_usernameList.append(key);
    }
    ui->user->addItems(m_usernameList);
}

// 当账号选择改变时，自动填充密码
void loginer::on_user_currentIndexChanged(const QString &arg1)
{
    QString pwd = settings->value("accounts/" + arg1).toString();
    ui->password->setText(pwd);
}
//鼠标移动事件
void loginer::mousePressEvent(QMouseEvent *event) {
        if (event->button() == Qt::LeftButton && !childAt(event->pos()) || (ui->label_gif->geometry().contains(event->pos()))) {
            m_dragPosition = event->globalPos() - frameGeometry().topLeft();
            event->accept();
        }
}
void loginer::mouseMoveEvent(QMouseEvent *event) {
    if ((event->buttons() & Qt::LeftButton) && !childAt(event->pos())|| ui->label_gif->geometry().contains(event->pos())) {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}
//注册按钮
void loginer::on_button_regist_clicked()
{
    regist *re=new regist(this);
    re->setWindowTitle("聊天室注册界面");
    re->setWindowIcon(QIcon(":/res/register.ico"));
    this->hide();
    re->show();


}
//登录按钮
void loginer::on_button_login_clicked()
{
    // 获取用户名输入框的文本并去除首尾空格
    m_User = ui->user->currentText().trimmed();
    // 获取密码输入框的文本
    m_Pwd = ui->password->text();

    // 用户名密码输入检查
    if (m_User == "") {
        // 若用户名为空，弹出错误消息框
        QMessageBox::critical(this,"ERROR","用户名为空");
    }
    else if (m_Pwd == "") {
        // 若密码为空，弹出错误消息框
        QMessageBox::critical(this,"ERROR","密码为空");
    }
    else if(db.searchUserData(m_User,m_Pwd)) {
        //检查是否勾选记住密码
        if (ui->checkBox_passwd->isChecked()) {
            saveAccountInfo(m_User, m_Pwd);
        }
        //检查是否勾选自动登录
        if (ui->checkBox_passwd->isChecked()){
            autologin=1;
        }
        else autologin=0;
        this->close();
        // 若用户名和密码在数据库中匹配,进入主窗体
        MainWindow *w=new MainWindow();
        w->setUsername(getUser());
        w->setPassword(getPwd());
        if(w->initConnect()){
            w->show();
        }
        else {
            delete w;
            exit(0);
        }
        w->setWindowIcon(QIcon(":/res/myicon.ico"));
    }
    else {
        // 若用户名和密码不匹配，弹出错误消息框
        QMessageBox::critical(this,"ERROR","账号信息错误");
    }
}
//按钮槽函数
void loginer::on_pushButton_exit_clicked()
{
    close();
}

void loginer::on_pushButton_min_clicked()
{
    showMinimized();
}
