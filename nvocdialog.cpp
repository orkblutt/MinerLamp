#include "nvocdialog.h"
#include "ui_nvocdialog.h"
#include <QDebug>

nvOCDialog::nvOCDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::nvOCDialog)
{
    ui->setupUi(this);


    _nvapi = new nvidiaAPI();



    unsigned int deviceNumber = _nvapi->getGPUCount();
    if(deviceNumber)
    {
        for(unsigned int i = 0; i < deviceNumber; i++)
        {
            ui->comboBoxDevice->addItem(QString("device " + QString::number(i)));
        }
int value = _nvapi->getPowerLimit(0);
qDebug() << value;
        ui->horizontalSliderPowerPercent->setValue(value);
    }


}

nvOCDialog::~nvOCDialog()
{
    delete ui;

    delete _nvapi;
}

void nvOCDialog::on_horizontalSliderPowerPercent_valueChanged(int value)
{
    ui->lcdNumberPowerPercent->display(value);
}

void nvOCDialog::on_horizontalSliderGpuOffset_valueChanged(int value)
{
    ui->lcdNumberGpuOffset->display(value);
}

void nvOCDialog::on_horizontalSliderMemOffset_valueChanged(int value)
{
    ui->lcdNumberMemClock->display(value);
}
