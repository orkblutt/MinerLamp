#include "leddialog.h"
#include "ui_leddialog.h"

LEDDialog::LEDDialog(unsigned short hash, unsigned short share, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LEDDialog),
    _hashingLEDIntensity(hash),
    _sharingLEDIntensity(share)
{
    ui->setupUi(this);
    setFixedSize(size());
    ui->spinBoxLEDHashIntensity->setValue(hash);
    ui->spinBoxLEDShareIntensity->setValue(share);
}

LEDDialog::~LEDDialog()
{
    delete ui;
}

void LEDDialog::getValues(unsigned short &hash, unsigned short &share)
{
    hash = _hashingLEDIntensity;
    share = _sharingLEDIntensity;
}

void LEDDialog::on_spinBoxLEDHashIntensity_valueChanged(int arg1)
{
    _hashingLEDIntensity = arg1;
}

void LEDDialog::on_spinBoxLEDShareIntensity_valueChanged(int arg1)
{
    _sharingLEDIntensity = arg1;
}
