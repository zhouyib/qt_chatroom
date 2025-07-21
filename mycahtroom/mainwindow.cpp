#include "mainwindow.h"
#include "ui_mainwindow.h"

// 初始化成员变量
void MainWindow::initMember()
{
    // 设置客户端的处理程序为当前对象
    m_Client.setHandler(this);
}
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    server = new tcpserver(this);

    //隐藏标题栏
    setWindowFlags(Qt::FramelessWindowHint);

    // 将焦点设置到消息输入框，并为其安装事件过滤器
    ui->inputEditor->setFocus();
    ui->inputEditor->installEventFilter(this);
    ui->inputEditor->setPlaceholderText("按回车换行，按ctrl+enter发送消息。");
    // 将消息编辑器设置为只读模式
    ui->msgEditor->setReadOnly(true);
    //ui->msgEditor->appendPlainText("你好");
    //读取qss样式文件
    QString qss;
    QFile qssFile(":/interface/interfae_main.qss");
    qssFile.open(QFile::ReadOnly);
    if(qssFile.isOpen())
    {
        qss=QLatin1String(qssFile.readAll());
        qDebug()<<qss;
        this->setStyleSheet(qss);
        qssFile.close();
    }
    // 设置列表部件不接受焦点
    ui->listWidget->setFocusPolicy(Qt::NoFocus);
    initMember();
    // 连接TCP服务器的sendFileName信号到getFileName槽函数，用于获取文件名
    connect(server,SIGNAL(sendFileName(QString)),this,SLOT(getFileName(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 获取文件名的槽函数
void MainWindow::getFileName(QString name)
{
    // 输出调试信息，表示新文件来了
    qDebug() << "新文件来了";
    // 发送包含文件名的消息
    //sendMessage();
}
// 初始化与服务器的连接
bool MainWindow::initConnect()
{
    // 尝试连接到指定的服务器地址和端口
    if (m_Client.connectTo(IP, 8080)) {
        // 若连接成功，创建一个登录文本消息
        QString ipaddr= QHostInfo::localHostName();
        TextMessage tm("LGIN", usr + '\r' + pwd + 'r' +ipaddr);
        // 客户端发送登录消息
        m_Client.send(tm);
        return true;
    } else {
        // 若连接失败，弹出错误消息框
        QMessageBox::critical(this, "Error", "无法连接服务器");
        return false;
    }
}


// 设置用户名
void MainWindow::setUsername(QString username)
{
    // 将传入的用户名赋值给成员变量
    usr = username;

}

// 设置密码
void MainWindow::setPassword(QString password)
{
    // 将传入的密码赋值给成员变量
    pwd = password;
}

// 处理接收到的消息
void MainWindow::handle(QTcpSocket &obj, TextMessage &message)
{
    qDebug()<<"recive:"<<message.data();
    // 根据消息类型进行不同的处理
    if (message.type() == "CONN") {
        // 处理连接成功消息
    } else if (message.type() == "DSCN") {
        // 处理断开连接消息
    } else if (message.type() == "LIOK") {
        // 若登录成功，弹出成功消息框
        QMessageBox::information(this, "OK", "已连接服务器");
    } else if (message.type() == "LIER") {
        // 若登录失败，弹出错误消息框
        QMessageBox::critical(this, "ERROR", "未连接服务器");
    } else if (message.type() == "MSGA") {
        // 若为消息广播，将消息添加到消息编辑器中
        QStringList datas = message.data().split(" ", QString::SkipEmptyParts);
        // 获取当前时间并格式化为字符串
        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        // 设置聊天记录显示框的文本颜色为蓝色，字体为Times New Roman，大小为12
        //4、创建TextCharFormat对象
        QTextCharFormat fmt;
        //5、设置文本颜色
        fmt.setForeground(QColor(Qt::blue));
        //6、设置颜色
        ui->msgEditor->mergeCurrentCharFormat(fmt);
        ui->msgEditor->setFont(QFont("Times New Roman",12));
        // 在聊天记录显示框中添加发送者的用户名和时间
        ui->msgEditor->appendPlainText("[ " + datas[0] +" ]  " + datas[1] + datas[2]);
        // 在聊天记录显示框中添加消息内容
        //ui->msgEditor->appendPlainText(message.data());
    } else if (message.type() == "USER") {
        //ui->msgEditor->appendPlainText("欢迎用户 "+message.data());
        // 若为用户列表消息，将用户列表显示在列表部件中
        QStringList users = message.data().split("\r", QString::SkipEmptyParts);
        // 清空列表部件
        ui->listWidget->clear();
        // 遍历用户列表
        for (int i = 0; i < users.length(); i++) {
            // 创建列表项
            QListWidgetItem* item = new QListWidgetItem;

            if (item != nullptr) {
                // 设置列表项的文本为用户名
                item->setText(users[i]);
                // 设置列表项的初始选中状态为未选中
                item->setCheckState(Qt::Unchecked);
                // 将列表项添加到列表部件中
                ui->listWidget->addItem(item);
            }
        }
    } else if(message.type() == "TFILE"){
        hasPendingFile(message.username(),message.filename());
    }
}
// 处理接收到文件名，询问用户是否接收文件的函数
void MainWindow::hasPendingFile(QString userName,QString fileName)
{
    if(userName  == usr)
    {
        // 弹出询问框，询问用户是否接受文件
        int btn = QMessageBox::information(this,tr("接收文件"),
                                           tr("来自 %1 的文件:%2","是否接受")
                                              .arg(userName)
                                              .arg(fileName),
                                           QMessageBox::Yes,QMessageBox::No);
        if(btn == QMessageBox::Yes)
        {
            // 弹出文件保存对话框，获取用户选择的保存路径和文件名
            QString name = QFileDialog::getSaveFileName(0,tr("保存文件"),fileName);
            if(!name.isEmpty())
            {
                // 创建TCP客户端对象
                tcpclient *client = new tcpclient(this);
                // 设置要保存的文件名
                client->setFileName(name);
                // 设置服务器的IP地址
                client->setHostAddress(QHostAddress(IP));
                // 显示TCP客户端窗口
                client->show();
                // 输出调试信息，提示客户端已创建与服务端的链接
                qDebug() << "客户端已创建与服务端的链接" ;
            }
            else
            {
               // 此处代码逻辑可能有误，创建了TcpClient对象但未进行有效操作，可考虑删除
               tcpclient  *client = new tcpclient(this);
               client->localFile =new  QFile(fileName);
            }
        }
        else if(btn == QMessageBox::No){
            // 如果用户拒绝接收文件，发送拒绝消息

            // 此处代码逻辑可能有误，创建了TcpClient对象但未进行有效操作，可考虑删除
            tcpclient  *client = new tcpclient(this);
            client->localFile =new QFile(fileName);
        }
    }
}
//获取本机ip
QString MainWindow::getIP()
{
    // 获取所有网络接口的地址列表
    QList<QHostAddress > list = QNetworkInterface::allAddresses();
    // 遍历地址列表
    foreach (QHostAddress address, list) {
        // 如果地址协议是IPv4，返回该地址的字符串表示
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
            return address.toString();
    }
    // 如果未找到IPv4地址，返回空字符串（原代码返回0错误）
    return "";
}
//鼠标事件重写
void MainWindow::mousePressEvent(QMouseEvent *event)  {
        if (event->button() == Qt::LeftButton) {
            m_dragPosition = event->globalPos() - frameGeometry().topLeft();
            event->accept();
        }
    }
void MainWindow::mouseMoveEvent(QMouseEvent *event)  {
        if (event->buttons() & Qt::LeftButton) {
            move(event->globalPos() - m_dragPosition);
            event->accept();
        }
    }
// 事件过滤器函数，用于监听消息输入框的按键事件
bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->inputEditor) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *k = static_cast<QKeyEvent*>(event);

            // 检查是否同时按下 Ctrl 和 Enter 键
            if (k->key() == Qt::Key_Return && (k->modifiers() & Qt::ControlModifier)) {
                on_sendBtn_clicked();  // 触发发送操作
                return true;           // 吞掉事件，防止默认行为
            }
        }
    }
    // 其他情况交给默认处理（单独Enter键会换行）
    return QMainWindow::eventFilter(watched, event);
}


void MainWindow::on_sendBtn_clicked()
{
    if(ui->inputEditor->toPlainText() == "")
    {
        // 若为空，弹出警告框提示用户
        QMessageBox::warning(0,tr("警告"),tr("发送内容不能为空"),QMessageBox::Ok);
        return ;
    }
    // 构造要发送的消息文本，包含用户名、时间和输入内容
    QString text = usr + "  " + QTime::currentTime().toString("HH:mm:ss") + "\n" + "  " + ui->inputEditor->toPlainText() + "\n";
    // 创建一个消息广播文本消息
    TextMessage tm("MSGA", text);

    // 若消息发送成功，清空输入编辑器
       if (m_Client.send(tm))
       {
           ui->inputEditor->clear();
       }
}
void MainWindow::on_pushButton_exit_clicked()
{
    close();
}

void MainWindow::on_pushButton_min_clicked()
{
    showMinimized();
}

void MainWindow::on_pushButton_clear_clicked()
{
    ui->msgEditor->setPlainText("");
}


// 处理发送文件工具按钮点击事件的函数
void MainWindow::on_sendToolBtn_clicked()
{
    // 检查用户列表中是否有选中的用户
    if(ui->listWidget->selectedItems().isEmpty())
    {
        // 若没有选中用户，弹出警告框提示用户选择
        QMessageBox::warning(0, tr("选择用户"),
                             tr("请从用户列表选择要传送的用户!"), QMessageBox::Ok);
        return;
    }
    QString username=ui->listWidget->currentItem()->text();
    QString data=getIP();
    TextMessage ms("FILE" , data);
    m_Client.send(ms);
    // 显示 TCP 服务器窗口
    server->show();
    // 初始化 TCP 服务器
    server->initServer();
}

