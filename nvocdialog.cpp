#include "nvocdialog.h"
#include "ui_nvocdialog.h"
#include <QDebug>

nvOCDialog::nvOCDialog(nvidiaAPI *nvapi, QSettings *settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::nvOCDialog),
    _gpuIndex(0),
    _settings(settings),
    _nvapi(nvapi)
{
    ui->setupUi(this);

    unsigned int deviceNumber = _nvapi->getGPUCount();
    if(deviceNumber)
    {
        for(unsigned int i = 0; i < deviceNumber; i++)
        {
            nvCard card;
            card.gpuOffset = 0;
            card.memOffset = 0;
            card.powerOffset = 0;

            _cardList << card;
            ui->comboBoxDevice->addItem(QString("device " + QString::number(i)));
        }

        updateSliders(0);
    }

    _settings->beginGroup("nvoc");
    ui->checkBoxAllDevices->setChecked(_settings->value("nvoc_applyall").toBool());
    ui->checkBoxOCMinerStart->setChecked(_settings->value("nvoc_applyonstart").toBool());
    _settings->endGroup();
}

nvOCDialog::~nvOCDialog()
{
    delete ui;
}

void nvOCDialog::on_horizontalSliderPowerPercent_valueChanged(int value)
{
    ui->lcdNumberPowerPercent->display(value);
    _cardList[_gpuIndex].powerOffset = value;
}

void nvOCDialog::on_horizontalSliderGpuOffset_valueChanged(int value)
{
    ui->lcdNumberGpuOffset->display(value);
    _cardList[_gpuIndex].gpuOffset = value;
}

void nvOCDialog::on_horizontalSliderMemOffset_valueChanged(int value)
{
    ui->lcdNumberMemClock->display(value);
    _cardList[_gpuIndex].memOffset = value;
}


void nvOCDialog::on_comboBoxDevice_activated(int index)
{
   updateSliders(index);
   _gpuIndex = index;
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

void nvOCDialog::saveConfig()
{
    _settings->beginGroup("nvoc");
    _settings->setValue("nvoc_applyall", ui->checkBoxAllDevices->isChecked());
    _settings->setValue("nvoc_applyonstart", ui->checkBoxOCMinerStart->isChecked());

    for(int i = 0; i < _cardList.size(); i++)
    {
        _settings->setValue(QString("powerlimitoffset" + QString::number(i)), _cardList.at(i).powerOffset);
        _settings->setValue(QString("gpuoffset" + QString::number(i)), _cardList.at(i).gpuOffset);
        _settings->setValue(QString("memoffset" + QString::number(i)), _cardList.at(i).memOffset);
    }

    _settings->endGroup();
}


void nvOCDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    if(button == (QAbstractButton*)ui->buttonBox->button(QDialogButtonBox::Apply))
    {
        int gpu = ui->comboBoxDevice->currentIndex();
        if(ui->checkBoxAllDevices->isChecked())
        {
            for(unsigned int i = 0; i < _nvapi->getGPUCount(); i++)
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

        saveConfig();
    }
}
