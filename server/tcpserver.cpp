#include "tcpserver.h"


// TcpServer 类的构造函数
TcpServer::TcpServer(QWidget *parent) :
    // 初始化 UI、本地文件指针、TCP 服务器指针和客户端连接指针
   localFile(nullptr), tcpServer(nullptr), clientConnection(nullptr)
{
    tcpPort = 8888;  // 设置 TCP 端口号
    tcpServer = new QTcpServer();  // 创建 TCP 服务器对象

    // 当有新的客户端连接时，触发 sendMessage 槽函数
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(sendMessage()));
    initServer();  // 初始化服务器
    // 启动 TCP 服务器，监听所有可用的 IP 地址和指定的端口
    if (!tcpServer->listen(QHostAddress::Any, tcpPort))
    {
        // 若监听失败，打印错误信息并关闭窗口
        qDebug() << tcpServer->errorString() << "ni 好吧";
        return;
    }
    emit sendFileName(theFileName);  // 发出发送文件名的信号
}

// TcpServer 类的析构函数
TcpServer::~TcpServer()
{
    // 2. 关闭服务器
    if (tcpServer->isListening()) {
        tcpServer->close();
    }
}

// 初始化服务器的函数
void TcpServer::initServer()
{
    payloadSize = 64 * 1024;  // 设置每次发送的数据块大小为 64KB
    TotalBytes = 0;  // 初始化要发送的总字节数为 0
    bytestoWrite = 0;  // 初始化待发送的字节数为 0
    bytesWritten = 0;  // 初始化已发送的字节数为 0

    tcpServer->close();  // 关闭 TCP 服务器
    localFile = new QFile(fileName);
}

// 发送文件消息的函数
void TcpServer::sendMessage()
{
    qDebug() << "TCP 的链接已建立";
    // 获取下一个待处理的客户端连接
    clientConnection = tcpServer->nextPendingConnection();

    // 当有数据写入客户端连接时，触发 updateClientProgress 槽函数
    connect(clientConnection, SIGNAL(bytesWritten(qint64)), this, SLOT(updateClientProgress(qint64)));
    localFile = new QFile(fileName);  // 创建本地文件对象
    // 尝试以只读模式打开本地文件
    if (!localFile->open(QFile::ReadOnly))
    {
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
void TcpServer::updateClientProgress(qint64 numBytes)
{
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


    float useTime = time.elapsed();  // 获取已经过去的时间
    double speed = bytesWritten / useTime;  // 计算发送速度

    if (bytesWritten == TotalBytes)
    {
        localFile->close();  // 关闭本地文件
        tcpServer->close();  // 关闭 TCP 服务器
    }
}

// 处理对方拒绝接受文件的函数
void TcpServer::refused()
{
    tcpServer->close();  // 关闭 TCP 服务器
}
