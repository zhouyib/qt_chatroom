#include "clientdemo.h"

// ClientDemo 类的构造函数
// 接收一个 QObject 指针作为父对象，初始化成员变量 m_handler 为 nullptr
ClientDemo::ClientDemo(QObject* parent) : QObject(parent), m_handler(nullptr)
{
// 连接 m_client 的 connected 信号到 onConnected 槽函数，当客户端连接成功时触发
connect(&m_client, SIGNAL(connected()), this, SLOT(onConnected()));
// 连接 m_client 的 disconnected 信号到 onDisconnected 槽函数，当客户端断开连接时触发
connect(&m_client, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
// 连接 m_client 的 readyRead 信号到 onDataReady 槽函数，当客户端有数据可读时触发
connect(&m_client, SIGNAL(readyRead()), this, SLOT(onDataReady()));
// 连接 m_client 的 bytesWritten 信号到 onBytesWritten 槽函数，当客户端有数据写入时触发
connect(&m_client, SIGNAL(bytesWritten(qint64)), this, SLOT(onBytesWritten(qint64)));
}

// 处理客户端连接成功的槽函数
void ClientDemo::onConnected()
{
// 若 m_handler 不为空
if ( m_handler != nullptr ) {
    // 创建一个 TextMessage 对象 conn，消息类型为 "CONN"，内容为服务器的地址和端口信息
    TextMessage conn("CONN", m_client.peerAddress().toString() + ":" + QString::number(m_client.peerPort()));
    // 调用 m_handler 的 handle 函数处理该连接消息
    m_handler->handle(m_client,conn);
}
}

// 处理客户端断开连接的槽函数
void ClientDemo::onDisconnected()
{
// 重置消息组装器
m_assembler.reset();
// 若 m_handler 不为空
if ( m_handler != nullptr ) {
    // 创建一个 TextMessage 对象 dscn，消息类型为 "DSCN"，内容为空
    TextMessage dscn("DSCN", "");
    // 调用 m_handler 的 handle 函数处理该断开连接消息
    m_handler->handle(m_client,dscn);
}
}

// 处理客户端有数据可读的槽函数
void ClientDemo::onDataReady()
{
    // 直接读取所有可用数据，而不是固定大小的缓冲区
    QByteArray data = m_client.readAll();

    if(!data.isEmpty()) {
        QSharedPointer<TextMessage> ptm = nullptr;
        m_assembler.prepare(data.constData(), data.size());

        while((ptm = m_assembler.assemble()) != nullptr) {
            if(m_handler != nullptr) {
                m_handler->handle(m_client, *ptm);
            }
        }
    }
}

// 处理客户端数据写入完成的槽函数
void ClientDemo::onBytesWritten(qint64 bytes)
{
// 忽略已写入的字节数，目前不做处理
(void)bytes;
}

// 连接到指定 IP 地址和端口的服务器
bool ClientDemo::connectTo(QString ip, int port)
{
// 调用 m_client 的 connectToHost 函数尝试连接到指定的服务器
m_client.connectToHost(ip, static_cast<quint16>(port));
// 等待连接成功，返回连接结果
return m_client.waitForConnected();
}

// 发送 TextMessage 消息
qint64 ClientDemo::send(TextMessage& message) {
    QByteArray ba = message.serialize().toUtf8(); // 直接使用UTF-8编码
    return m_client.write(ba);
}
// 获取客户端套接字中可用的字节数
qint64 ClientDemo::available()
{
// 调用 m_client 的 bytesAvailable 函数返回可用字节数
return m_client.bytesAvailable();
}

// 关闭客户端连接
void ClientDemo::close()
{
// 调用 m_client 的 close 函数关闭客户端套接字
m_client.close();
}

// 判断客户端连接是否有效
bool ClientDemo::isValid()
{
// 调用 m_client 的 isValid 函数返回连接是否有效的结果
return m_client.isValid();
}

// 设置消息处理器
void ClientDemo::setHandler(TxtMsgHandler* handler)
{
// 将 m_handler 设置为传入的 TxtMsgHandler 对象
m_handler = handler;
}
