#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void onSubmitClicked();
    void onModifyClicked(bool clicked);

signals:
    void submit(const QByteArray&);

private:
    Ui::Dialog *ui;
    QPushButton* mModifyBtn;
    QPushButton* mSubmitBtn;
};

#endif // DIALOG_H
