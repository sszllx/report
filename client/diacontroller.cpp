#include "diacontroller.h"
#include "dialog.h"
#include "config.h"

#include <QDebug>

DiaController::DiaController(QObject *parent) : QObject(parent),
    m_dialog(new Dialog),
    m_cfg(NULL)
{
    connect(m_dialog.data(), &Dialog::submit, this, &DiaController::updateContentList);
}

void DiaController::setConfig(const Config *config)
{
    m_cfg = config;
}

void DiaController::onShowDialog()
{
    m_dialog->show();
}

void DiaController::updateContentList(const QByteArray &str)
{
    QList<QByteArray> sl = str.split('\t');
    qDebug() << QByteArray::fromBase64(sl[0]) << QByteArray::fromBase64(sl[1]);
}
