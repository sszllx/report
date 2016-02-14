#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QSharedPointer>

class Dialog;
typedef QSharedPointer<Dialog> SmartDialog;

class DiaController : public QObject
{
    Q_OBJECT
public:
    explicit DiaController(QObject *parent = 0);

signals:

public slots:
    void onShowDialog();

private:
    SmartDialog m_dialog;
};

#endif // CONTROLLER_H
