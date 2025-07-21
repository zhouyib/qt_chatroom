#include "serverdemo.h"
#include <QHostAddress>
#include <QTcpSocket>
#include <QObjectList>
#include <QDebug>
#include <QString>

// 构造函数，初始化 ServerDemo 类的对象
// parent 是父对象，用于内存管理
ServerDemo::ServerDemo(QObject* parent) : QObject(parent), m_handler(nullptr)
{
    // 连接 m_server 的 newConnection 信号到当前对象的 onNewConnection 槽函数
    // 当有新的客户端连接到服务器时，会触发 onNewConnection 函数
    connect(&m_server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}

// 处理新的客户端连接
void ServerDemo::onNewConnection()
{
    // 获取下一个待处理的客户端连接的套接字
    QTcpSocket* tcp = m_server.nextPendingConnection();
    // 创建一个新的文本消息组装器对象
    TxtMsgAssembler* assembler = new TxtMsgAssembler();

    // 将套接字和对应的消息组装器插入到映射表中
    m_map.insert(tcp, assembler);

    // 连接套接字的 connected 信号到当前对象的 onConnected 槽函数
    connect(tcp, SIGNAL(connected()), this, SLOT(onConnected()));
    // 连接套接字的 disconnected 信号到当前对象的 onDisconnected 槽函数
    connect(tcp, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    // 连接套接字的 readyRead 信号到当前对象的 onDataReady 槽函数
    connect(tcp, SIGNAL(readyRead()), this, SLOT(onDataReady()));
    // 连接套接字的 bytesWritten 信号到当前对象的 onBytesWritten 槽函数
    connect(tcp, SIGNAL(bytesWritten(qint64)), this, SLOT(onBytesWritten(qint64)));

    // 如果消息处理程序不为空
    if (m_handler != nullptr){
        // 创建一个新的文本消息，消息类型为 "CONN"，内容为客户端的地址和端口
        TextMessage msg("CONN", tcp->peerAddress().toString() + ":" + QString::number(tcp->peerPort()));
        // 调用消息处理程序处理该消息
        m_handler->handle(*tcp,msg);
    }
}

// 处理客户端连接成功事件
void ServerDemo::onConnected()
{
    // 目前此函数为空，可根据需求添加具体逻辑
}

// 处理客户端断开连接事件
void ServerDemo::onDisconnected()
{
    // 获取发送信号的套接字对象
    QTcpSocket* tcp = dynamic_cast<QTcpSocket*>(sender());

    // 如果套接字对象不为空
    if( tcp != nullptr )
    {
        // 从映射表中移除该套接字对应的消息组装器，并释放其内存
        delete m_map.take(tcp);
    }

    // 如果消息处理程序不为空
    if (m_handler != nullptr){
        // 创建一个新的文本消息，消息类型为 "DSCN"，内容为空
        TextMessage msg("DSCN", "");
        // 调用消息处理程序处理该消息
        m_handler->handle(*tcp,msg);
    }
}

// 处理客户端有数据可读事件
// 修改 onDataReady() 方法中的数据处理部分
void ServerDemo::onDataReady() {
    QTcpSocket* tcp = dynamic_cast<QTcpSocket*>(sender());

    if (tcp != nullptr) {
        TxtMsgAssembler* assembler = m_map.value(tcp);

        if (assembler != nullptr) {
            QByteArray data = tcp->readAll();  // 读取所有可用数据
            QSharedPointer<TextMessage> ptm = assembler->assemble(data.constData(), data.size());

            while (ptm != nullptr) {
                m_handler->handle(*tcp, *ptm);
                ptm = assembler->assemble();  // 尝试组装下一条消息
            }
        }
    }
}
// 处理数据写入完成事件
void ServerDemo::onBytesWritten(qint64 bytes)
{
    // 目前此函数只是简单忽略了写入的字节数，可根据需求添加具体逻辑
    (void)bytes;
}

// 启动服务器监听指定端口
bool ServerDemo::start(int port)
{
    // 用于记录启动是否成功的标志
    bool ret = true;

    // 如果服务器当前没有在监听
    if( !m_server.isListening() )
    {
        // 尝试在指定的地址和端口上启动监听
        ret = m_server.listen(QHostAddress::Any, static_cast<quint16>(port));
    }

    return ret;
}

// 停止服务器监听
void ServerDemo::stop()
{
    // 如果服务器当前正在监听
    if( m_server.isListening() )
    {
        // 关闭服务器监听
        m_server.close();
    }
}

// 析构函数，释放服务器占用的资源
ServerDemo::~ServerDemo()
{
    // 获取服务器的所有子对象列表
    const QObjectList& list = m_server.children();

    // 遍历子对象列表
    for(int i=0; i<list.length(); i++)
    {
        // 将子对象转换为 QTcpSocket 类型
        QTcpSocket* tcp = dynamic_cast<QTcpSocket*>(list[i]);

        // 如果转换成功
        if( tcp != nullptr )
        {
            // 关闭该套接字
            tcp->close();
        }
    }

    // 获取映射表中所有的消息组装器列表
    const QList<TxtMsgAssembler*>& al = m_map.values();

    // 遍历消息组装器列表
    for(int i=0; i<al.length(); i++)
    {
        // 释放每个消息组装器的内存
        delete al.at(i);
    }
}

// 设置消息处理程序
void ServerDemo::setHandler(TxtMsgHandler* handler)
{
    // 将传入的消息处理程序赋值给成员变量
    m_handler = handler;
}
