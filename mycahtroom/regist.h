#ifndef REGIST_H
#define REGIST_H

#include <QWidget>
#include "loginer.h"
#include "database.h"

namespace Ui {
class regist;
}

class regist : public QDialog
{
    Q_OBJECT

public:
    explicit regist(QDialog *parent = nullptr);
    ~regist();

private slots:
    void on_button_cancel_clicked();
    void on_button_regist_clicked();

private:
    Ui::regist *ui;
    DataBaseDemo db;

};



#endif // REGIST_H
