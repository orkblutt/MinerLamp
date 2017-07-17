#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>

namespace Ui {
class helpDialog;
}

class helpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit helpDialog(QWidget *parent = 0);
    ~helpDialog();

private slots:
    void on_pushButtonFinish_clicked();

private:
    Ui::helpDialog *ui;
};

#endif // HELPDIALOG_H
