#include "tcpserver.h"


// TcpServer 类的构造函数
tcpserver::tcpserver(QWidget *parent) :
    QDialog(parent),
    // 初始化 UI、本地文件指针、TCP 服务器指针和客户端连接指针
    ui(new Ui::tcpserver), localFile(nullptr), tcpServer(nullptr), clientConnection(nullptr)
{

    ui->setupUi(this);  // 设置 UI
    tcpPort = 6666;  // 设置 TCP 端口号
    tcpServer = new QTcpServer(this);  // 创建 TCP 服务器对象

    // 当有新的客户端连接时，触发 sendMessage 槽函数
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(sendMessage()));
    initServer();  // 初始化服务器
}

// TcpServer 类的析构函数
tcpserver::~tcpserver()
{
    delete ui;  // 释放 UI 对象的内存
}

// 初始化服务器的函数
void tcpserver::initServer()
{
    payloadSize = 64 * 1024;  // 设置每次发送的数据块大小为 64KB
    TotalBytes = 0;  // 初始化要发送的总字节数为 0
    bytestoWrite = 0;  // 初始化待发送的字节数为 0
    bytesWritten = 0;  // 初始化已发送的字节数为 0

    // 设置服务器状态标签，提示用户选择要传送的文件
    ui->serverStatusLabel->setText(tr("请选择要传送的文件"));
    ui->progressBar->reset();  // 重置进度条
    ui->serverOpenBtn->setEnabled(true);  // 启用打开文件按钮
    ui->serverSendBtn->setEnabled(false);  // 禁用发送文件按钮
    tcpServer->close();  // 关闭 TCP 服务器
    localFile = new QFile(fileName);
}

// 发送文件消息的函数
void tcpserver::sendMessage()
{
    qDebug() << "TCP 的链接已建立";
    ui->serverSendBtn->setEnabled(false);  // 禁用发送文件按钮

    // 获取下一个待处理的客户端连接
    clientConnection = tcpServer->nextPendingConnection();

    // 当有数据写入客户端连接时，触发 updateClientProgress 槽函数
    connect(clientConnection, SIGNAL(bytesWritten(qint64)), this, SLOT(updateClientProgress(qint64)));

    // 设置服务器状态标签，提示开始传送文件
    ui->serverStatusLabel->setText(tr("开始传送文件:\n %1!").arg(theFileName));

    localFile = new QFile(fileName);  // 创建本地文件对象
    // 尝试以只读模式打开本地文件
    if (!localFile->open(QFile::ReadOnly))
    {
        // 若打开文件失败，弹出警告框提示用户
        QMessageBox::warning(this, tr("应用程序"), tr("无法读取文件 %1:\n%2").arg(fileName).arg(localFile->errorString()));
        return;
    }

    TotalBytes = localFile->size();  // 获取文件的总字节数
    time.start();  // 启动计时器

    // 创建数据输出流，用于将数据写入 outBlock 字节数组
    QDataStream sendOut(&outBlock, QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_4_0);  // 设置数据输出流的版本
    time.start();  // 再次启动计时器

    // 获取文件名（去除路径）
    QString currentFile = fileName.right(fileName.size() - fileName.lastIndexOf('/') - 1);
    // 先写入两个占位的 qint64 类型数据和文件名
    sendOut << qint64(0) << qint64(0) << currentFile;

    TotalBytes += outBlock.size();  // 更新要发送的总字节数
    sendOut.device()->seek(0);  // 将文件指针移动到数据输出流的开头

    // 写入实际的总字节数和文件名所占字节数
    sendOut << TotalBytes << qint64((outBlock.size() - sizeof(qint64) * 2));

    // 计算待发送的字节数，并发送 outBlock 数据
    bytestoWrite = TotalBytes - clientConnection->write(outBlock);
    outBlock.resize(0);  // 清空 outBlock 字节数组
}

// 更新客户端进度的函数
void tcpserver::updateClientProgress(qint64 numBytes)
{
    qApp->processEvents();  // 处理应用程序的事件
    bytesWritten += (int)numBytes;  // 更新已发送的字节数

    if (bytestoWrite > 0)
    {
        // 从本地文件中读取数据，每次读取的数据量不超过待发送字节数和数据块大小的最小值
        outBlock = localFile->read(qMin(bytestoWrite, payloadSize));
        // 发送读取的数据，并更新待发送的字节数
        bytestoWrite -= (int)clientConnection->write(outBlock);
        outBlock.resize(0);  // 清空 outBlock 字节数组
    }
    else
    {
        localFile->close();  // 关闭本地文件
    }

    ui->progressBar->setMaximum(TotalBytes);  // 设置进度条的最大值为总字节数
    ui->progressBar->setValue(bytesWritten);  // 设置进度条的当前值为已发送的字节数

    float useTime = time.elapsed();  // 获取已经过去的时间
    double speed = bytesWritten / useTime;  // 计算发送速度

    // 更新服务器状态标签，显示已发送的文件大小、速度、总大小、已用时间和估计剩余时间
    ui->serverStatusLabel->setText(tr("已发送 %1MB( %2MB/s)"
                                      "\n共%3MB 已用时:%4秒\n估计剩余时间:%5秒")
                                   .arg(bytesWritten / (1024 * 1024))
                                   .arg(speed * 1000 / (1024 * 1024), 0, 'f', 2)
                                   .arg(TotalBytes / (1024 * 1024))
                                   .arg(useTime / 1000, 0, 'f', 0)
                                   .arg(TotalBytes / speed / 1000 - useTime / 1000, 0, 'f', 0));

    if (bytesWritten == TotalBytes)
    {
        localFile->close();  // 关闭本地文件
        tcpServer->close();  // 关闭 TCP 服务器
        // 更新服务器状态标签，提示文件传送成功
        ui->serverStatusLabel->setText(tr("传送文件: %1成功").arg(theFileName));
    }
}

// 处理打开文件按钮点击事件的函数
void tcpserver::on_serverOpenBtn_clicked()
{
    // 弹出文件选择对话框，让用户选择要发送的文件
    fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
    {
        // 获取文件名（去除路径）
        theFileName = fileName.right(fileName.size() - fileName.lastIndexOf('/') - 1);
        // 更新服务器状态标签，显示要发送的文件名
        ui->serverStatusLabel->setText(tr("要发送的文件为:%1").arg(theFileName));
        ui->serverSendBtn->setEnabled(true);  // 启用发送文件按钮
        ui->serverOpenBtn->setEnabled(false);  // 禁用打开文件按钮
    }
    localFile = new QFile(fileName);
}

// 处理发送文件按钮点击事件的函数
void tcpserver::on_serverSendBtn_clicked()
{
    // 启动 TCP 服务器，监听所有可用的 IP 地址和指定的端口
    if (!tcpServer->listen(QHostAddress::Any, tcpPort))
    {
        // 若监听失败，打印错误信息并关闭窗口
        qDebug() << tcpServer->errorString() << "ni 好吧";
        close();
        return;
    }
    ui->serverSendBtn->setEnabled(false);  // 禁用发送文件按钮
    // 更新服务器状态标签，提示等待对方接受
    ui->serverStatusLabel->setText(tr("等待对方的接受......"));
    emit sendFileName(theFileName);  // 发出发送文件名的信号
}

// 处理关闭按钮点击事件的函数
void tcpserver::on_serverCloseBtn_clicked()
{
    if (tcpServer->isListening())
    {
        qDebug() << "点击了关闭按钮";
        tcpServer->close();  // 关闭 TCP 服务器
        if (localFile->isOpen())
        {
            localFile->close();  // 关闭本地文件
        }
        if (clientConnection && clientConnection->state() == QAbstractSocket::ConnectedState)
            clientConnection->abort();  // 中止客户端连接
    }
    close();  // 关闭窗口
}

// 处理对方拒绝接受文件的函数
void tcpserver::refused()
{
    tcpServer->close();  // 关闭 TCP 服务器
    // 更新服务器状态标签，提示对方拒绝接受
    ui->serverStatusLabel->setText(tr("对方拒绝接受!!!"));
}

// 处理窗口关闭事件的函数
void tcpserver::closeEvent(QCloseEvent *)
{
    on_serverCloseBtn_clicked();  // 调用关闭按钮点击事件处理函数
}
