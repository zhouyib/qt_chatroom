#include "serverhandler.h"

// 获取所有在线用户的 ID，以字符串形式返回，每个 ID 后跟随一个回车符
QString ServerHandler::getOnlineUserId()
{
    // 用于存储所有在线用户 ID 的字符串
    QString ret = "";

    // 遍历存储节点信息的列表
    for (int i = 0; i < m_nodeList.length(); i++) {
        // 获取当前节点
        Node* n = m_nodeList.at(i);

        // 检查节点对应的套接字是否有效（即用户是否在线）
        if (n->socket != nullptr) {
            // 将在线用户的 ID 添加到结果字符串，并在后面添加回车符
            ret += n->id + '\r';
        }
    }

    return ret;
}

// 向所有在线用户发送文本消息
// 确保所有消息发送都使用UTF-8编码
void ServerHandler::sendToAllOnlineUser(TextMessage &tm) {
    QByteArray ba = tm.serialize().toUtf8();  // 确保UTF-8编码

    for (int i = 0; i < m_nodeList.length(); i++) {
        Node* n = m_nodeList.at(i);
        if (n->socket != nullptr) {
            n->socket->write(ba);
        }
    }
}
// 构造函数
ServerHandler::ServerHandler()
{

}
ServerHandler::~ServerHandler(){

}

// 处理接收到的文本消息
void ServerHandler::handle(QTcpSocket &obj, TextMessage &message)
{
    // 输出套接字对象的地址，用于调试
    qDebug() << &obj;
    // 输出消息的类型，用于调试
    qDebug() << message.type();
    // 输出消息的数据内容，用于调试
    qDebug() << message.data();

    // 处理类型为 "MSGA" 的消息，即群发消息
    if (message.type() == "MSGA") {
        // 调用 sendToAllOnlineUser 函数将该消息发送给所有在线用户
        sendToAllOnlineUser(message);
    }

    // 处理类型为 "CONN" 的消息，目前此部分为空，可根据需求添加处理逻辑
    if (message.type() == "CONN") {

    }

    // 处理类型为 "DSCN" 的消息，即用户断开连接的消息
    if (message.type() == "DSCN") {
        // 遍历存储节点信息的列表
        for (int i = 0; i < m_nodeList.length(); i++) {
            // 获取当前节点
            Node* n = (m_nodeList.at(i));

            // 检查当前节点的套接字是否与发送断开消息的套接字相同
            if (n->socket == &obj) {
                // 将该节点的套接字置为 nullptr，表示用户已离线
                n->socket = nullptr;
                break;
            }
        }

        // 创建一个新的文本消息，类型为 "USER"，数据为当前所有在线用户的 ID
        TextMessage tm("USER", getOnlineUserId());
        // 向所有在线用户发送该消息，通知在线用户列表的更新
        sendToAllOnlineUser(tm);
    }

    // 处理类型为 "LGIN" 的消息，即用户登录消息
    if (message.type() == "LGIN") {
        // 获取消息的数据内容
        QString data = QString::fromUtf8(message.data().toUtf8()); // 确保正确解码
        // 查找数据中回车符的位置
        int index = data.indexOf('\r');
        // 从数据中提取用户 ID
        QString id = data.mid(0, index);
        // 从数据中提取用户密码
        QString pwd = data.mid(index + 1);
        //提取ip
        QString ipaddr = data.mid(index + 2);
        // 用于存储登录结果的字符串
        QString result = "";

        // 重置索引，用于查找用户是否已存在于节点列表中
        index = -1;

        // 遍历存储节点信息的列表
        for (int i = 0; i < m_nodeList.length(); i++) {
            // 检查当前节点的 ID 是否与登录用户的 ID 相同
            if (id == m_nodeList.at(i)->id) {
                // 记录该用户在列表中的索引
                index = i;
                break;
            }
        }

        // 如果用户不在节点列表中
        if (index == -1) {
            // 创建一个新的节点对象
            Node* newNode = new Node();

            // 检查节点对象是否创建成功
            if (newNode != nullptr) {
                // 设置新节点的 ID
                newNode->id = id;
                // 设置新节点的密码
                newNode->pwd = pwd;
                // 将新节点的套接字指向当前登录用户的套接字
                newNode->socket = &obj;
                // 将新节点添加到节点列表中
                m_nodeList.append(newNode);
                // 登录结果设置为成功
                result = "LIOK";
            } else {
                // 登录结果设置为失败
                result = "LIER";
            }
        } else {
            // 获取已存在用户的节点
            Node* n = m_nodeList.at(index);

            // 检查用户输入的密码是否与节点中存储的密码相同
            if (pwd == n->pwd) {
                // 将该节点的套接字指向当前登录用户的套接字
                n->socket = &obj;
                // 登录结果设置为成功
                result = "LIOK";
            } else {
                // 登录结果设置为失败
                result = "LIER";
            }
        }

        // 将登录结果以文本消息的形式发送给用户
        //obj.write(TextMessage(result, id).serialize().toUtf8());

        // 如果登录成功
        if (result == "LIOK") {
            // 创建一个新的文本消息，类型为 "USER"，数据为当前所有在线用户的 ID
            TextMessage tm("USER", getOnlineUserId());
            // 向所有在线用户发送该消息，通知在线用户列表的更新
            sendToAllOnlineUser(tm);
        }
    }
    else if(message.type() == "FILE"){
        // 连接TCP服务器的sendFileName信号到getFileName槽函数，用于获取文件名
        //connect(server,SIGNAL(sendFileName(QString)),this,SLOT(getFileName(QString)));

//        QString username=message.data();
//        QStringList datas = message.data().split(" ", QString::SkipEmptyParts);
//        QString ipaddr;
//        // 遍历存储节点信息的列表
//        for (int i = 0; i < m_nodeList.length(); i++) {
//            // 检查当前节点的 ID 是否与登录用户的 ID 相同
//            if (datas[0] == m_nodeList.at(i)->id) {
//                // 记录该用户在列表中的索引
//                ipaddr=m_nodeList.at(i)->ipaddr;
//                break;
//            }
//        }

        hasPendingFile("datas[0]","ipaddr");
        //发送文件
        //server = new TcpServer();
        // 初始化 TCP 服务器
        //server->initServer();
    }
}
// 处理接收到文件
void ServerHandler::hasPendingFile(QString username,QString ipaddr)
{
    // 创建TCP客户端对象
    TcpClient *client = new TcpClient();
    // 设置要保存的文件名
    client->setFileName(username);
    // 设置服务器的IP地址
    client->setHostAddress(QHostAddress("192.168.28.196"));
    // 输出调试信息，提示客户端已创建与服务端的链接
    qDebug() << "客户端已创建与服务端的链接" ;
}
// 获取文件名的槽函数
void ServerHandler::getFileName(QString name)
{
    // 输出调试信息，表示新文件来了
    qDebug() << "新文件来了";
    // 发送包含文件名的消息
    //sendMessage();
}
