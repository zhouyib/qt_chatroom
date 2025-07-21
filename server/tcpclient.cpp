#include "tcpclient.h"


// TcpClient 类的构造函数，用于初始化对象
TcpClient::TcpClient(QWidget *parent) :
    tcpClient(nullptr),  // 初始化 TCP 套接字指针为 nullptr
    localFile(nullptr)  // 初始化本地文件指针为 nullptr
{
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
TcpClient::~TcpClient()
{

}

// 设置要保存的本地文件的文件名
void TcpClient::setFileName(QString fileName)
{
    localFile = new QFile(fileName);  // 创建一个新的 QFile 对象，用于操作本地文件
}

// 设置服务器的主机地址，并发起新的连接
void TcpClient::setHostAddress(QHostAddress address)
{
    hostAddress = address;  // 保存服务器的主机地址
    newConnect();  // 发起新的连接
}

// 发起新的 TCP 连接
void TcpClient::newConnect()
{
    blockSize = 0;  // 初始化数据块大小为 0
    tcpClient->abort();  // 中止当前的 TCP 连接
    tcpClient->connectToHost(hostAddress,tcpPort);  // 连接到指定的服务器地址和端口
    if(!tcpClient->waitForConnected(10000)) { // 10秒超时
        qDebug() << "Connection timeout";
        // 处理超时逻辑
        tcpClient->abort();  // 强制关闭连接
        return ;
    }
    time.start();  // 启动计时器，用于记录接收文件的时间
}

// 读取从服务器发送过来的消息
void TcpClient::readMessage()
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


    // 计算下载速度和剩余时间
    double speed = bytesReceived / useTime;  // 计算下载速度（字节/毫秒）

    // 当文件接收完成时
    if(bytesReceived == TotalBytes)
    {
       localFile ->close();  // 关闭本地文件
       tcpClient->close();  // 关闭 TCP 连接
    }
}

// 处理 TCP 套接字的错误
void TcpClient::displayError(QAbstractSocket::SocketError  sockError)
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
