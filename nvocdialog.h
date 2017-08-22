#ifndef NVOCDIALOG_H
#define NVOCDIALOG_H

#include <QDialog>
#include <QAbstractButton>

#include "nvidianvml.h"
#include "nvidiaapi.h"
#include <QSettings>


class nvCard
{
public:
    int powerOffset;
    int memOffset;
    int gpuOffset;
};

namespace Ui {
class nvOCDialog;
}

class nvOCDialog : public QDialog
{
    Q_OBJECT

public:
    explicit nvOCDialog(QSettings& settings, QWidget *parent = 0);
    ~nvOCDialog();

private slots:
    void on_horizontalSliderPowerPercent_valueChanged(int value);

    void on_horizontalSliderGpuOffset_valueChanged(int value);

    void on_horizontalSliderMemOffset_valueChanged(int value);

    void on_comboBoxDevice_activated(int index);

    void on_buttonBox_clicked(QAbstractButton *button);

private:

    void updateSliders(unsigned int gpu);

    Ui::nvOCDialog *ui;

    QSettings _settings;
    nvidiaNVML* _nvml;

    nvidiaAPI* _nvapi;

    QList<nvCard> _cardList;
};

#endif // NVOCDIALOG_H
