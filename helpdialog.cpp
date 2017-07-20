#include "helpdialog.h"
#include "ui_helpdialog.h"

helpDialog::helpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::helpDialog)
{
    ui->setupUi(this);
    ui->plainTextEdit->appendHtml("<html>");
    ui->plainTextEdit->appendHtml("<h2>Simple Qt GUI for running ethminer safely.</h2><br>");
    ui->plainTextEdit->appendHtml("It will restart ethminer for you in case of error...");
    ui->plainTextEdit->appendHtml("<u>https://github.com/orkblutt/MinerLamp</u><br>");
    ui->plainTextEdit->appendHtml("You'll need ethminer you can download from:");
    ui->plainTextEdit->appendHtml("<u>https://github.com/ethereum-mining/ethminer</u><br>");
    ui->plainTextEdit->appendHtml("If you want to donate you can at this address:");
    ui->plainTextEdit->appendHtml("<b>0xa07a8c9975145bb5371e8b3c31acb62ad9d0698e</b><br><br>");
    ui->plainTextEdit->appendHtml("<br>Feel free to send me comment at <b>orkblutt@msn.com</b>");
    ui->plainTextEdit->appendHtml("</html>");
}

helpDialog::~helpDialog()
{
    delete ui;
}

void helpDialog::on_pushButtonFinish_clicked()
{
    close();
}
