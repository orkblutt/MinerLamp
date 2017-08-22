#ifndef NVOCDIALOG_H
#define NVOCDIALOG_H

#include <QDialog>

#include "nvidianvml.h"
#include "nvidiaapi.h"

namespace Ui {
class nvOCDialog;
}

class nvOCDialog : public QDialog
{
    Q_OBJECT

public:
    explicit nvOCDialog(QWidget *parent = 0);
    ~nvOCDialog();

private slots:
    void on_horizontalSliderPowerPercent_valueChanged(int value);

    void on_horizontalSliderGpuOffset_valueChanged(int value);

    void on_horizontalSliderMemOffset_valueChanged(int value);

    void on_comboBoxDevice_activated(int index);



private:

    void updateSliders(unsigned int gpu);

    Ui::nvOCDialog *ui;

    nvidiaNVML* _nvml;

    nvidiaAPI* _nvapi;
};

#endif // NVOCDIALOG_H
