#include "serverdemo.h"
#include <QHostAddress>
#include <QTcpSocket>
#include <QObjectList>
#include <QDebug>

// ServerDemo类的构造函数
ServerDemo::ServerDemo(QObject* parent) : QObject(parent), m_handler(nullptr)
{
    // 将m_server的newConnection信号连接到onNewConnection槽函数
    connect(&m_server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}

// 处理新连接的槽函数
void ServerDemo::onNewConnection()
{
    // 获取下一个待处理的连接
    QTcpSocket* tcp = m_server.nextPendingConnection();
    // 创建一个新的TxtMsgAssembler对象
    TxtMsgAssembler* assembler = new TxtMsgAssembler();

    // 将套接字和组装器插入到映射中
    m_map.insert(tcp, assembler);

    // 将套接字的信号连接到相应的槽函数
    connect(tcp, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(tcp, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(tcp, SIGNAL(readyRead()), this, SLOT(onDataReady()));
    connect(tcp, SIGNAL(bytesWritten(qint64)), this, SLOT(onBytesWritten(qint64)));
}

// 处理连接建立信号的槽函数
void ServerDemo::onConnected()
{
    // 目前为空实现
}

// 处理连接断开信号的槽函数
void ServerDemo::onDisconnected()
{
    // 获取发送信号的套接字
    QTcpSocket* tcp = dynamic_cast<QTcpSocket*>(sender());

    if (tcp != nullptr)
    {
        // 从映射中移除并删除对应的组装器
        delete m_map.take(tcp);
    }
}

// 处理数据可读信号的槽函数
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
// 处理数据已写入信号的槽函数
void ServerDemo::onBytesWritten(qint64 bytes)
{
    // 目前忽略已写入的字节数
    (void)bytes;
}

// 启动服务器，监听指定端口
bool ServerDemo::start(int port)
{
    bool ret = true;

    if (!m_server.isListening())
    {
        // 开始监听指定地址和端口
        ret = m_server.listen(QHostAddress::Any, static_cast<quint16>(port));
    }

    return ret;
}

// 停止服务器
void ServerDemo::stop()
{
    if (m_server.isListening())
    {
        // 关闭服务器监听
        m_server.close();
    }
}

// ServerDemo类的析构函数
ServerDemo::~ServerDemo()
{
    // 获取服务器的子对象列表
    const QObjectList& list = m_server.children();

    for(int i = 0; i < list.length(); i++)
    {
        // 尝试将子对象转换为套接字
        QTcpSocket* tcp = dynamic_cast<QTcpSocket*>(list[i]);

        if (tcp != nullptr)
        {
            // 关闭套接字
            tcp->close();
        }
    }

    // 获取映射中所有的组装器列表
    const QList<TxtMsgAssembler*>& al = m_map.values();

    for(int i = 0; i < al.length(); i++)
    {
        // 删除组装器对象
        delete al.at(i);
    }
}

// 设置文本消息处理器
void ServerDemo::setHandler(TxtMsgHandler* handler)
{
    m_handler = handler;
}
