#ifndef LEDDIALOG_H
#define LEDDIALOG_H

#include <QDialog>

namespace Ui {
class LEDDialog;
}

class LEDDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LEDDialog(unsigned short hash, unsigned short share, QWidget *parent = 0);
    ~LEDDialog();

    void getValues(unsigned short& hash, unsigned short& share);

private slots:
    void on_spinBoxLEDHashIntensity_valueChanged(int arg1);

    void on_spinBoxLEDShareIntensity_valueChanged(int arg1);

private:
    Ui::LEDDialog *ui;

    unsigned short _hashingLEDIntensity;
    unsigned short _sharingLEDIntensity;
};

#endif // LEDDIALOG_H
