#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QSharedPointer>

class Config;
class Dialog;
typedef QSharedPointer<Dialog> SmartDialog;

class DiaController : public QObject
{
    Q_OBJECT
public:
    explicit DiaController(QObject *parent = 0);

    void setConfig(const Config* config);

signals:

public slots:
    void onShowDialog();

private:
    SmartDialog m_dialog;
    Config* m_cfg;
};

#endif // CONTROLLER_H
