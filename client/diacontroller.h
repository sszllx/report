#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QSharedPointer>

class Config;
class Dialog;
typedef QSharedPointer<Dialog> SmartDialog;

typedef struct {
    QString content;
    int duration;
} DataItem;

class DiaController : public QObject
{
    Q_OBJECT
public:
    explicit DiaController(QObject *parent = 0);

    void setConfig(const Config* config);

signals:

public slots:
    void onShowDialog();
    void updateContentList(const QByteArray& str);

private:
    SmartDialog m_dialog;
    const Config* m_cfg;
    QHash<QString, DataItem> m_data_hash;
};

#endif // CONTROLLER_H
