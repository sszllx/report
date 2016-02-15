#include "config.h"

#include <QtCore>

#define DATAPATH "./data"

Config::Config(QObject *parent) : QObject(parent)
{
    QFile file(DATAPATH);

    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        throw QString("config is opened failed");
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        processLine(line);
    }
}

void Config::processLine(const QString &str)
{

}

