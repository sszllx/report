#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    QComboBox* cb = ui->duration;
    cb->addItem("10分钟");
    cb->addItem("20分钟");
    cb->addItem("30分钟");
    cb->addItem("40分钟");
    cb->addItem("50分钟");
    cb->addItem("1小时");
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_submit_clicked()
{
    qApp->exit();
}
