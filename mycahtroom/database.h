#ifndef DATABASEDEMO_H
#define DATABASEDEMO_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QMessageBox>
#include <QDebug>

class DataBaseDemo
{
public:
    DataBaseDemo();
    ~DataBaseDemo();  // 添加析构函数

    void initDataBase();
    bool searchUserData(QString user, QString pwd);
    bool insertUser(QString user, QString pwd);
private:
    QSqlDatabase db;
};

#endif // DATABASEDEMO_H
