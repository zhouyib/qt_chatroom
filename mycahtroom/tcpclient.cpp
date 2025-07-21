#include "tcpclient.h"


// TcpClient 类的构造函数，用于初始化对象
tcpclient::tcpclient(QWidget *parent) :
    QDialog(parent),  // 调用父类 QDialog 的构造函数
    tcpClient(nullptr),  // 初始化 TCP 套接字指针为 nullptr
    localFile(nullptr),  // 初始化本地文件指针为 nullptr
    ui(new Ui::tcpclient)  // 创建 UI 对象
{
    ui->setupUi(this);  // 设置 UI

    // 初始化接收文件所需的变量
    TotalBytes = 0;  // 要接收文件的总字节数
    bytesReceived = 0;  // 已经接收的字节数
    fileNameSize = 0;  // 文件名的字节数

    tcpClient = new QTcpSocket(this);  // 创建一个新的 TCP 套接字对象
    tcpPort = 6666;  // 设置 TCP 端口号为 6666

    // 连接信号与槽
    // 当 TCP 套接字有数据可读时，调用 readMessage 槽函数
    connect(tcpClient,SIGNAL(readyRead()),
            this,SLOT(readMessage()));
    // 当 TCP 套接字出现错误时，调用 displayError 槽函数
    connect(tcpClient,SIGNAL(error(QAbstractSocket::SocketError)),
            this,SLOT(displayError(QAbstractSocket::SocketError)));
}

// TcpClient 类的析构函数，用于释放资源
tcpclient::~tcpclient()
{
    delete ui;  // 删除 UI 对象
}

// 设置要保存的本地文件的文件名
void tcpclient::setFileName(QString fileName)
{
    localFile = new QFile(fileName);  // 创建一个新的 QFile 对象，用于操作本地文件
}

// 设置服务器的主机地址，并发起新的连接
void tcpclient::setHostAddress(QHostAddress address)
{
    hostAddress = address;  // 保存服务器的主机地址
    newConnect();  // 发起新的连接
}

// 发起新的 TCP 连接
void tcpclient::newConnect()
{
    blockSize = 0;  // 初始化数据块大小为 0
    tcpClient->abort();  // 中止当前的 TCP 连接
    tcpClient->connectToHost(hostAddress,tcpPort);  // 连接到指定的服务器地址和端口
    time.start();  // 启动计时器，用于记录接收文件的时间
}

// 读取从服务器发送过来的消息
void tcpclient::readMessage()
{
    QDataStream in(tcpClient);  // 创建一个数据输入流，用于从 TCP 套接字读取数据
    in.setVersion(QDataStream::Qt_4_0);  // 设置数据输入流的版本为 Qt 4.0

    float useTime = time.elapsed();  // 获取从开始接收文件到现在所经过的时间（毫秒）

    // 处理文件头信息
    if(bytesReceived <= sizeof(qint64)*2)
    {
        // 读取文件总字节数和文件名的字节数
        if((tcpClient->bytesAvailable() >= sizeof(qint64)*2) && (fileNameSize ==0))
        {
            in >> TotalBytes >> fileNameSize;  // 从数据输入流中读取文件总字节数和文件名的字节数
            bytesReceived += sizeof(qint64)*2;  // 更新已接收的字节数
        }
        // 读取文件名
        if((tcpClient->bytesAvailable() >= fileNameSize) && (fileNameSize!=0))
        {
            in >> fileName;  // 从数据输入流中读取文件名
            bytesReceived += fileNameSize;  // 更新已接收的字节数
            // 尝试以只写模式打开本地文件
            if(! localFile->open(QFile::WriteOnly))
            {
                // 如果打开文件失败，弹出警告框提示用户
                QMessageBox::warning(this,tr("应用程序"),tr("无法读取文件 %1:\n%2.")
                                     .arg(fileName)
                                     .arg(localFile->errorString()));
                return ;  // 结束函数
            }
            else
                return ;  // 结束函数
        }
    }

    // 处理文件数据
    if(bytesReceived < TotalBytes)
    {
        bytesReceived += tcpClient->bytesAvailable();  // 更新已接收的字节数
        inBlock = tcpClient->readAll();  // 读取 TCP 套接字中的所有数据
        localFile->write(inBlock);  // 将读取的数据写入本地文件
        inBlock.resize(0);  // 清空数据块
    }

    // 更新进度条
    ui->progressBar->setMaximum(TotalBytes);  // 设置进度条的最大值为文件总字节数
    ui->progressBar->setValue(bytesReceived);  // 设置进度条的当前值为已接收的字节数

    // 计算下载速度和剩余时间
    double speed = bytesReceived / useTime;  // 计算下载速度（字节/毫秒）
    ui->tcpClientStatusLabel->setText(tr("已接收 %1MB( %2MB/s)"
                                         "\n共%3MB 已用时:%4秒\n估计剩余时间:%5秒")
                                      .arg(bytesReceived / (1024*1024))  // 已接收的文件大小（MB）
                                      .arg(speed *1000/(1024*1024),0,'f',2)  // 下载速度（MB/s）
                                      .arg(TotalBytes / (1024*1024))  // 文件总大小（MB）
                                      .arg(useTime/1000,0,'f',0)  // 已用时间（秒）
                                      .arg(TotalBytes/speed/1000 - useTime/1000,0,'f',0 ));  // 估计剩余时间（秒）

    // 当文件接收完成时
    if(bytesReceived == TotalBytes)
    {
       localFile ->close();  // 关闭本地文件
       tcpClient->close();  // 关闭 TCP 连接
       ui->tcpClientStatusLabel->setText(tr("接收文件: %1完毕").arg(fileName));  // 更新状态标签，提示文件接收完毕
    }
}

// 处理 TCP 套接字的错误
void tcpclient::displayError(QAbstractSocket::SocketError  sockError)
{
    switch (sockError) {
    case QAbstractSocket::RemoteHostClosedError:
        // 如果是远程主机关闭连接的错误，不做处理
        break;
    default:
        // 对于其他错误，打印错误信息到调试输出
        qDebug() << tcpClient->errorString();
    }
}

// 处理取消按钮的点击事件
void tcpclient::on_tcpClientCancleBtn_clicked()
{
    tcpClient->abort();  // 中止当前的 TCP 连接
    if(localFile->isOpen())
    {
        localFile->close();  // 如果本地文件已打开，关闭它
    }
    close();  // 关闭当前窗口
    // ui->~TcpClient(); 这行代码有误，不能直接调用 UI 对象的析构函数，应删除
}

// 处理关闭按钮的点击事件
void tcpclient::on_tcpClientCloseBtn_clicked()
{
    on_tcpClientCancleBtn_clicked();  // 调用取消按钮的点击事件处理函数
}

// 处理窗口关闭事件
void tcpclient::closeEvent(QCloseEvent *)
{
    on_tcpClientCloseBtn_clicked();  // 调用关闭按钮的点击事件处理函数
}
