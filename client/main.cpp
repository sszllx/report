#include <QApplication>
#include "diacontroller.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    Dialog w;
//    w.show();

    DiaController dctl;
    dctl.onShowDialog();

    return a.exec();
}
