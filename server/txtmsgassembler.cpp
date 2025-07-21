#include "txtmsgassembler.h"

// 构造函数，初始化 TxtMsgAssembler 对象
// parent 是父对象，用于内存管理
TxtMsgAssembler::TxtMsgAssembler(QObject* parent) : QObject(parent)
{
    // 这里可以添加一些初始化逻辑，但目前为空
}

// 清除当前消息组装器的状态，将类型、长度和数据重置为空
void TxtMsgAssembler::clear()
{
    m_type = "";
    m_length = 0;
    m_data = "";
}

// 从队列中取出指定数量的字符并拼接成一个字符串
// n 是要取出的字符数量
// 返回拼接好的字符串
QString TxtMsgAssembler::fetch(int n)
{
    QString ret = "";

    // 循环从队列中取出字符并添加到结果字符串中
    for(int i = 0; i < n; i++)
    {
        ret += m_queue.dequeue();
    }

    return ret.toUtf8();
}

// 将接收到的数据添加到队列中，用于后续消息组装
// data 是接收到的数据指针
// len 是数据的长度
void TxtMsgAssembler::prepare(const char* data, int len)
{
    if( data != nullptr )
    {
        // 遍历接收到的数据，将每个字符添加到队列中
        for(int i = 0; i < len; i++)
        {
            m_queue.enqueue(data[i]);
        }
    }
}

// 尝试组装一个完整的文本消息
// 返回一个指向组装好的文本消息的智能指针，如果组装失败则返回空指针
QSharedPointer<TextMessage> TxtMsgAssembler::assemble()
{
    TextMessage* ret = nullptr;
    bool tryMakeMsg = false;

    // 如果消息类型还未确定
    if( m_type == "" )
    {
        // 尝试从队列中提取消息类型和长度
        tryMakeMsg = makeTypeAndLength();
    }
    else
    {
        // 如果消息类型已经确定，直接尝试组装消息
        tryMakeMsg = true;
    }

    if( tryMakeMsg )
    {
        // 尝试根据类型和长度组装消息
        ret = makeMessage();
    }

    if( ret != nullptr )
    {
        // 如果成功组装出消息，清除当前组装器的状态
        clear();
    }

    return QSharedPointer<TextMessage>(ret);
}

// 先将接收到的数据添加到队列中，然后尝试组装一个完整的文本消息
// data 是接收到的数据指针
// len 是数据的长度
// 返回一个指向组装好的文本消息的智能指针，如果组装失败则返回空指针
QSharedPointer<TextMessage> TxtMsgAssembler::assemble(const char* data, int len)
{
    // 先将数据添加到队列中
    prepare(data, len);

    // 然后尝试组装消息
    return assemble();
}

// 从队列中提取消息类型和长度
// 返回是否成功提取的布尔值
bool TxtMsgAssembler::makeTypeAndLength() {
    // 需要至少8个字节(类型4字节+长度4字节)
    if (m_queue.size() < 8) return false;

    // 读取类型(4字节)
    QByteArray typeBytes;
    for (int i = 0; i < 4; i++) {
        typeBytes.append(m_queue.dequeue());
    }
    m_type = QString::fromUtf8(typeBytes).trimmed();
    // 读取长度(4字节十六进制)
    QByteArray lenBytes;
    for (int i = 0; i < 4; i++) {
        lenBytes.append(m_queue.dequeue());
    }
    bool ok = false;
    m_length = QString::fromUtf8(lenBytes).toInt(&ok, 16);
    if (!ok || m_length < 0) {
        clear();
        return false;
    }

    return true;
}


// 根据已经确定的消息类型和长度，尝试从队列中取出足够的数据来组装一个完整的消息
// 返回一个指向组装好的文本消息的指针，如果组装失败则返回空指针
TextMessage* TxtMsgAssembler::makeMessage() {
    if (m_type.isEmpty()) return nullptr;

    // 收集足够的字节数据
    QByteArray dataBytes;
    while (!m_queue.isEmpty() && dataBytes.size() < m_length) {
        dataBytes.append(m_queue.dequeue());
    }

    if (dataBytes.size() == m_length) {
        QString data = QString::fromUtf8(dataBytes);
        return new TextMessage(m_type, data);
    }

    return nullptr;
}
// 重置消息组装器的状态，清除所有数据
void TxtMsgAssembler::reset()
{
    // 清除类型、长度和数据
    clear();
    // 清空队列
    m_queue.clear();
}
