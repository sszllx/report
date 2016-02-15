#include "dialog.h"
#include "ui_dialog.h"

#include <QDebug>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    QComboBox* cb = ui->unit;
    cb->addItem("min");
    cb->addItem("hour");

    mModifyBtn = ui->modify;
    connect(mModifyBtn, &QPushButton::clicked, this, &Dialog::onModifyClicked);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::onSubmitClicked()
{
    QTextEdit* edit = ui->content;
    QString content = edit->toPlainText();
    QLineEdit* durationEdit = ui->duration;
    QString durationStr = durationEdit->text();
    QComboBox* cb = ui->unit;
    QString unit = cb->currentText();

    bool ok;
    int duration = durationStr.toInt(&ok);
    if (!ok) {
        throw QString("input error");
    }

    int minutes = 0;
    if (unit == "min") {
        minutes += duration;
    } else if (unit == "hour") {
        minutes += duration * 60;
    }


    emit submit();
}

void Dialog::onModifyClicked(bool clicked)
{

}
