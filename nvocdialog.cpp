#include "nvocdialog.h"
#include "ui_nvocdialog.h"
#include <QDebug>

nvOCDialog::nvOCDialog(QSettings &settings, QWidget *parent) :
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
        updateSliders(0);
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


void nvOCDialog::on_comboBoxDevice_activated(int index)
{
   updateSliders(index);
}

void nvOCDialog::updateSliders(unsigned int gpu)
{
    int plimit = _nvapi->getPowerLimit(gpu);
    int gpuoffset = _nvapi->getGPUOffset(gpu);
    int memoffset = _nvapi->getMemOffset(gpu);

    ui->horizontalSliderPowerPercent->setValue(plimit);
    ui->horizontalSliderGpuOffset->setValue(gpuoffset);
    ui->horizontalSliderMemOffset->setValue(memoffset);
}



void nvOCDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    if(button == (QAbstractButton*)ui->buttonBox->button(QDialogButtonBox::Apply))
    {
        int gpu = ui->comboBoxDevice->currentIndex();
        if(ui->checkBoxAllDevices->isChecked())
        {
            for(int i = 0; i < _nvapi->getGPUCount(); i++)
            {
                _nvapi->setPowerLimitPercent(i, ui->horizontalSliderPowerPercent->value());
                _nvapi->setGPUOffset(i, ui->horizontalSliderGpuOffset->value());
                _nvapi->setMemClockOffset(i, ui->horizontalSliderMemOffset->value());
            }
        }
        else
        {
            _nvapi->setPowerLimitPercent(gpu, ui->horizontalSliderPowerPercent->value());
            _nvapi->setGPUOffset(gpu, ui->horizontalSliderGpuOffset->value());
            _nvapi->setMemClockOffset(gpu, ui->horizontalSliderMemOffset->value());
        }


    }
}
