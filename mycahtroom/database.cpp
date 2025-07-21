#include "database.h"
#include <QSqlQuery>
#include <QDebug>

// 构造函数，在创建 DataBaseDemo 对象时调用，用于初始化数据库
DataBaseDemo::DataBaseDemo()
{
    initDataBase();
}
DataBaseDemo::~DataBaseDemo()
{
    if(db.isOpen()) {
        db.close();
    }
}
// 初始化数据库的函数
void DataBaseDemo::initDataBase()
{
    // 添加 SQLite 数据库驱动
    db = QSqlDatabase::addDatabase("QSQLITE");

    // 设置数据库名称
    db.setDatabaseName("MyDataBase.db");
    // 尝试打开数据库
    if (!db.open()) {
        // 若打开失败，输出错误信息
        qDebug() << "Error: Failed to connect database." ;
    }
    else {
        // 若打开成功，输出成功信息
        qDebug() << "Succeed to connect database." ;
    }

    // 创建 QSqlQuery 对象，用于执行 SQL 语句
    QSqlQuery sql_query(db);

    // 执行 SQL 语句创建 User 表
    if (!sql_query.exec("create table if not exists User(Username varchar(20), Password varchar(20))")) {
        // 若创建表失败，输出错误信息
        qDebug() << "Error: Fail to create table.";
    }
    else {
        // 若创建表成功，输出成功信息
        qDebug() << "Table created!";
    }

    // 准备 SQL 查询语句，使用占位符防止 SQL 注入
    sql_query.prepare("select * from User where Username = :username");
    sql_query.bindValue(":username", "test");
    // 执行查询语句
    if(!sql_query.exec() || !sql_query.next()){
        // 若查询失败或者没有结果，插入初始数据
        insertUser("test", "123456");
        insertUser("admin","123456");
        insertUser("guest","123456");
        // 输出初始化表成功的信息
        qDebug() << "Init Table successfully" ;
    }

}

// 根据用户名和密码查询用户数据的函数
bool DataBaseDemo::searchUserData(QString user, QString pwd)
{
    // 创建 QSqlQuery 对象，用于执行 SQL 语句
    QSqlQuery sql_query(db);
    // 准备 SQL 查询语句，使用占位符防止 SQL 注入
    sql_query.prepare("select * from User where Username = :username");
    sql_query.bindValue(":username", user);
    // 执行查询语句
    if (!sql_query.exec()) {
        // 若查询失败，输出错误信息并返回 false
        qDebug()<<"Error: Fail to searchUserData";
        return false;
    }
    else {
        // 若查询成功，获取结果集的下一行
        if(sql_query.next()){
            // 获取用户名和密码
            QString m_user = sql_query.value(0).toString();
            QString m_pwd = sql_query.value(1).toString();
            // 输出用户名和密码
            qDebug()<<m_user;
            qDebug()<<m_pwd;

            // 比较密码是否匹配
            if (m_pwd == pwd) {
                // 若匹配，输出成功信息并返回 true
                qDebug()<<"searchUserdata successfully";
                return true;
            }
        }
        // 若不匹配，返回 false
        return false;
    }
}

// 向 User 表中插入用户数据的函数
bool DataBaseDemo::insertUser(QString user, QString pwd)
{
    // 创建 QSqlQuery 对象，用于执行 SQL 语句
    QSqlQuery sql_query(db);

    // 准备 SQL 插入语句，使用占位符防止 SQL 注入
    sql_query.prepare("INSERT INTO User VALUES(:username, :password)");
    sql_query.bindValue(":username", user);
    sql_query.bindValue(":password", pwd);
    // 执行插入语句
    if (!sql_query.exec()) {
        // 若插入失败，输出错误信息并返回 false
        qDebug() << "Error: Fail to insert data";
        return false;
    }
    else {
        // 若插入成功，输出成功信息并返回 true
        qDebug() << "inserted successfully!";
        return true;
    }
}
