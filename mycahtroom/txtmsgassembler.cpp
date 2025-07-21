#include "txtmsgassembler.h"

// 构造函数，初始化TxtMsgAssembler对象
TxtMsgAssembler::TxtMsgAssembler(QObject* parent) : QObject(parent)
{
    // 此处可以添加更多初始化逻辑，当前构造函数没有额外操作
}

// 清除已缓存的消息类型、长度和数据
void TxtMsgAssembler::clear()
{
    // 将消息类型置为空字符串
    m_type = "";
    // 将消息长度置为0
    m_length = 0;
    // 将消息数据置为空字符串
    m_data = "";
}

// 从队列中取出指定数量的字符并组合成字符串
QString TxtMsgAssembler::fetch(int n)
{
    QString ret = "";

    // 循环从队列中取出字符并添加到返回的字符串中
    for(int i = 0; i < n; i++)
    {
        ret += m_queue.dequeue();
    }

    return ret;
}

// 将传入的数据添加到队列中
void TxtMsgAssembler::prepare(const char* data, int len)
{
    if( data != nullptr )
    {
        // 遍历传入的数据，将每个字符添加到队列中
        for(int i = 0; i < len; i++)
        {
            m_queue.enqueue(data[i]);
        }
    }
}

// 尝试组装一个完整的文本消息
QSharedPointer<TextMessage> TxtMsgAssembler::assemble()
{
    TextMessage* ret = nullptr;
    bool tryMakeMsg = false;

    // 如果消息类型为空，尝试从队列中提取类型和长度信息
    if( m_type == "" )
    {
        tryMakeMsg = makeTypeAndLength();
    }
    else
    {
        // 若已有类型信息，直接尝试组装消息
        tryMakeMsg = true;
    }

    if( tryMakeMsg )
    {
        // 尝试根据类型和长度信息组装消息
        ret = makeMessage();
    }

    if( ret != nullptr )
    {
        // 若成功组装消息，清除已缓存的类型、长度和数据
        clear();
    }

    // 使用智能指针管理消息对象
    return QSharedPointer<TextMessage>(ret);
}

// 先将传入的数据添加到队列，再尝试组装消息
QSharedPointer<TextMessage> TxtMsgAssembler::assemble(const char* data, int len)
{
    // 将传入的数据添加到队列
    prepare(data, len);

    // 尝试组装消息
    return assemble();
}

// 从队列中提取消息类型和长度信息
bool TxtMsgAssembler::makeTypeAndLength()
{
    // 检查队列中是否有足够的字符（至少8个）来提取类型和长度
    bool ret = (m_queue.length() >= 8);

    if( ret )
    {
        QString len = "";

        // 从队列中取出前4个字符作为消息类型
        m_type = fetch(4);

        // 从队列中取出接下来的4个字符作为消息长度的字符串表示
        len = fetch(4);

        // 将长度字符串转换为整数，并检查转换是否成功
        m_length = len.trimmed().toInt(&ret, 16);

        if( !ret )
        {
            // 若转换失败，清除已缓存的类型、长度和数据
            clear();
        }
    }

    return ret;
}

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
// 重置组装器，清除所有缓存信息和队列中的数据
void TxtMsgAssembler::reset()
{
    // 清除已缓存的类型、长度和数据
    clear();
    // 清空队列
    m_queue.clear();
}
