#include "diacontroller.h"

#include "config.h"
#include "dialog.h"

DiaController::DiaController(QObject *parent) : QObject(parent),
    m_dialog(new Dialog),
    m_cfg(NULL)
{

}

void DiaController::setConfig(const Config *config)
{
    m_cfg = config;
}

void DiaController::onShowDialog()
{
    m_dialog->show();
}

