#include "diacontroller.h"

#include "dialog.h"

DiaController::DiaController(QObject *parent) : QObject(parent),
    m_dialog(new Dialog)
{

}

void DiaController::onShowDialog()
{
    m_dialog->show();
}

