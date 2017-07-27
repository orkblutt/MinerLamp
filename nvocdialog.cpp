#include "nvocdialog.h"
#include "ui_nvocdialog.h"
#include <QDebug>

nvOCDialog::nvOCDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::nvOCDialog)
{
    ui->setupUi(this);


    _nvapi = new nvidiaAPI();


    ui->horizontalSliderPowerPercent->setValue(_nvapi->getPowerLimit(0));

    unsigned int gpuclock = _nvapi->getGpuClock(0);
    qDebug() << gpuclock;


/*
   qDebug() <<

   _nvapi->setPowerLimitPercent(0, 80);
   qDebug() << _nvapi->getPowerLimit(0);

   _nvapi->setPowerLimitPercent(0, 100);
   qDebug() << _nvapi->getPowerLimit(0);
   */


   //_nvapi->setLED(0, 100);
  // _nvapi->setMemClock(0, 500);



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
