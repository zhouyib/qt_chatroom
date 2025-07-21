#include "textmessage.h"

// 默认构造函数，初始化数据为空字符串和类型为空字符串
TextMessage::TextMessage(QObject *parent) : QObject(parent)
{
    m_data = "";
    m_type = "";
}

// 带参数的构造函数，用于创建特定类型和数据的文本消息
TextMessage::TextMessage(QString type, QString data, QObject *parent) : QObject (parent)
{
    // 去除类型字符串两端的空白字符
    m_type = type.trimmed();
    // 将类型字符串调整为固定长度 4，不足的用空格填充
    m_type.resize(4, ' ');
    // 保存消息数据
    m_data = data;
}

// 获取消息类型的函数
QString TextMessage::type()
{
    return m_type;
}

// 获取消息数据长度的函数
int TextMessage::length()
{
    return m_data.length();
}
//获取文件发送人
QString TextMessage::username(){
    return m_username;
}
//文件名称
QString TextMessage::filename(){
    return m_filename;
}
// 获取消息数据内容的函数
QString TextMessage::data()
{
    return m_data;
}

// 将消息序列化为字符串的函数
QString TextMessage::serialize() {
    QByteArray typeUtf8 = m_type.toUtf8().rightJustified(4, ' '); // 类型转为UTF-8，右对齐补空格至4字节
    QByteArray dataUtf8 = m_data.toUtf8(); // 数据转为UTF-8
    QString lenHex = QString::number(dataUtf8.size(), 16); // 计算UTF-8字节数，转为16进制
    lenHex = lenHex.rightJustified(4, ' '); // 长度字段补空格至4字节
    return QString(typeUtf8 + lenHex + dataUtf8);
}
// 将序列化的字符串反序列化为消息对象的函数
bool TextMessage::unserialize(QString s) {
    if (s.length() < 8) return false;
    QByteArray typeData = s.left(4).toUtf8();
    m_type = QString::fromUtf8(typeData);

    QString lenStr = s.mid(4, 4);
    int dataLen = lenStr.toInt(nullptr, 16); // 解析16进制长度
    if (s.length() < 8 + dataLen) return false;

    QByteArray dataArray = s.mid(8, dataLen).toUtf8();
    m_data = QString::fromUtf8(dataArray);
    return true;
}
