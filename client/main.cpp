#include <QApplication>
#include <QDebug>
#include <QFile>

#include "config.h"
#include "diacontroller.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    Dialog w;
//    w.show();

    Config cfg;
    DiaController dctl;
    try {
        cfg.init();
        dctl.onShowDialog();
        dctl.setConfig(&cfg);
    } catch (QString errString) {
        qDebug() << errString;
    }

    return a.exec();
}
