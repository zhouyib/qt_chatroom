#include "textmessage.h"

// 默认构造函数，用于创建一个空的文本消息对象
// parent 为父对象，用于内存管理
TextMessage::TextMessage(QObject *parent) : QObject(parent)
{
    // 初始化消息数据为空字符串
    m_data = "";
    // 初始化消息类型为空字符串
    m_type = "";
}

// 带参数的构造函数，用于创建一个具有指定类型和数据的文本消息对象
// type 为消息类型，data 为消息数据，parent 为父对象
TextMessage::TextMessage(QString type, QString data, QObject *parent) : QObject (parent)
{
    // 去除消息类型前后的空白字符
    m_type = type.trimmed();
    // 将消息类型的长度调整为 4 个字符，不足部分用空格填充
    m_type.resize(4, ' ');
    // 保存消息数据
    m_data = data;
}

// 获取消息类型
// 返回值为消息类型的字符串
QString TextMessage::type()
{
    return m_type.toUtf8();
}

// 获取消息数据的长度
// 返回值为消息数据的字符数量
int TextMessage::length()
{
    return m_data.length();
}

// 获取消息数据
// 返回值为消息数据的字符串
QString TextMessage::data()
{
    return m_data.toUtf8();
}

// TextMessage::serialize 修改后
QString TextMessage::serialize() {
    QByteArray dataUtf8 = m_data.toUtf8(); // 转为UTF-8编码
    QString typePadded = m_type.rightJustified(4, ' '); // 类型右对齐补空格
    QString lenStr = QString::number(dataUtf8.length(), 16).rightJustified(4, '0');  // 长度是字节数

    return typePadded + lenStr + QString::fromUtf8(dataUtf8);
}

// 在 unserialize() 方法中正确处理UTF-8编码
bool TextMessage::unserialize(QString s) {
    if (s.length() < 8) return false;

    m_type = s.mid(0, 4).trimmed();
    bool ok = false;
    int dataLen = s.mid(4, 4).toInt(&ok, 16);  // 长度是字节数

    if (!ok || dataLen < 0 || (8 + dataLen) > s.toUtf8().length()) {
        return false;
    }

    // 按字节数截取UTF-8数据
    QByteArray utf8Data = s.toUtf8().mid(8, dataLen);
    m_data = QString::fromUtf8(utf8Data);

    return true;
}
