#include "nvocdialog.h"
#include "ui_nvocdialog.h"
#include <QDebug>

nvOCDialog::nvOCDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::nvOCDialog)
{
    ui->setupUi(this);

    _nvml = new nvidiaNVML();

    _nvml->getMaxSupportedMemClock(0);

}

nvOCDialog::~nvOCDialog()
{
    delete ui;
    delete _nvml;
}
