#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "minerprocess.h"
#include "helpdialog.h"

#include <QDebug>
#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>
#include <QCloseEvent>

#define MINERPATH "minerpath"
#define MINERARGS "minerargs"
#define AUTORESTART "autorestart"
#define MAX0MHS "max0mhs"
#define RESTARTDELAY "restartdelay"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _isMinerRunning(false),
    _isStartStoping(false),
    _errorCount(0)
{

    _process = new MinerProcess();
    _settings = new QSettings("Orkblutt binaries factory", "Miner's Lamp");

    ui->setupUi(this);

    ui->lineEditMinerPath->setText(_settings->value(MINERPATH).toString());
    ui->lineEditArgs->setText(_settings->value(MINERARGS).toString());
    ui->checkBoxRestart->setChecked(_settings->value(AUTORESTART).toBool());
    ui->spinBoxMax0MHs->setValue(_settings->value(MAX0MHS).toInt());
    ui->spinBoxDelay->setValue(_settings->value(RESTARTDELAY).toInt());

    _process->setLogControl(ui->textEdit);

    connect(_process, &MinerProcess::emitStarted, this, &MainWindow::onMinerStarted);
    connect(_process, &MinerProcess::emitStoped, this, &MainWindow::onMinerStoped);
    connect(_process, &MinerProcess::emitHashRate, this, &MainWindow::onHashrate);
    connect(_process, &MinerProcess::emitError, this, &MainWindow::onError);

    createActions();

    createTrayIcon();

    setIcon();

    connect(_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);

    _trayIcon->show();

    setupEditor();
}

MainWindow::~MainWindow()
{
    _settings->setValue(MINERPATH, ui->lineEditMinerPath->text());
    _settings->setValue(MINERARGS, ui->lineEditArgs->text());
    _settings->setValue(AUTORESTART, ui->checkBoxRestart->isChecked());
    _settings->setValue(MAX0MHS, ui->spinBoxMax0MHs->value());
    _settings->setValue(RESTARTDELAY, ui->spinBoxDelay->value());

    _process->stop();

    delete _process;
    delete _settings;
    delete ui;
}

void MainWindow::setVisible(bool visible)
{
    _maximizeAction->setEnabled(!isMaximized());
    _restoreAction->setEnabled(isMaximized() || !visible);
    QMainWindow::setVisible(visible);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
#ifdef Q_OS_OSX
    if (!event->spontaneous() || !isVisible())
        return;
#endif
    if (_trayIcon->isVisible())
    {
        hide();
        event->ignore();
        QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Information);
        _trayIcon->showMessage("Miner's lamp still running", _isMinerRunning ? "Ethminer is running" : "Ethminer isn't running", icon, 2 * 1000);

    }
}

void MainWindow::setIcon()
{
    QIcon icon(":/icons/eth_miner_lamp.ico");
    _trayIcon->setIcon(icon);
    _trayIcon->setToolTip("Miner's lamp");

    setWindowIcon(icon);
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        QWidget::showNormal();
        break;
    default:
        ;
    }
}

void MainWindow::createActions()
{
    _minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(_minimizeAction, &QAction::triggered, this, &QWidget::hide);

    _maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(_maximizeAction, &QAction::triggered, this, &QWidget::showMaximized);

    _restoreAction = new QAction(tr("&Restore"), this);
    connect(_restoreAction, &QAction::triggered, this, &QWidget::showNormal);

    _quitAction = new QAction(tr("&Close"), this);
    connect(_quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

}

void MainWindow::createTrayIcon()
{
    _trayIconMenu = new QMenu(this);
    _trayIconMenu->addAction(_minimizeAction);
    _trayIconMenu->addAction(_maximizeAction);
    _trayIconMenu->addAction(_restoreAction);
    _trayIconMenu->addSeparator();
    _trayIconMenu->addAction(_quitAction);

    _trayIcon = new QSystemTrayIcon(this);
    _trayIcon->setContextMenu(_trayIconMenu);
}

void MainWindow::setupEditor()
{
    QFont font;
    font.setFamily("Tahoma");
    font.setFixedPitch(true);
    font.setPointSize(8);

    ui->textEdit->setFont(font);

    _highlighter = new Highlighter(ui->textEdit->document());
}

void MainWindow::on_pushButton_clicked()
{
    if(!_isStartStoping) // avoid to start/stop more than once on dbl clic
    {
        _isStartStoping = true;

        if(!_isMinerRunning)
        {
            _process->setMax0MHs(ui->spinBoxMax0MHs->value());
            _process->setRestartDelay(ui->spinBoxDelay->value());
            _process->setRestartOption(ui->checkBoxRestart->isChecked());
            _process->start(ui->lineEditMinerPath->text(), ui->lineEditArgs->text());
        }
        else
            _process->stop();
    }
}

void MainWindow::onMinerStarted()
{
    ui->pushButton->setText("Stop mining");
    _isMinerRunning = true;
    _isStartStoping = false;
}

void MainWindow::onMinerStoped()
{
    ui->pushButton->setText("Start mining");
    _isMinerRunning = false;
    _isStartStoping = false;
    this->setWindowTitle(QString("Miner's Lamp"));
}

void MainWindow::onHashrate(QString &hashrate)
{
    this->setWindowTitle(QString("Miner's Lamp - " + hashrate + " - Restart count: " + QString::number(_errorCount)));
}

void MainWindow::onError()
{
    _errorCount++;
    _trayIcon->showMessage("Miner's lamp report", "An error has been detected in ehtminer.\n" + ui->checkBoxRestart->isChecked() ? "Miner's lamp restarted it automaticaly" : "Check the autorestart checkbox if you want Miner's lamp to restart it on error");
}

void MainWindow::on_checkBoxRestart_clicked(bool checked)
{
    _process->setRestartOption(checked);
}

void MainWindow::on_spinBoxMax0MHs_valueChanged(int arg1)
{
    _process->setMax0MHs(arg1);
}

void MainWindow::on_spinBoxDelay_valueChanged(int arg1)
{
    _process->setRestartDelay(arg1);
}

void MainWindow::on_checkBoxOnlyShare_clicked(bool checked)
{
    _process->setShareOnly(checked);
}

void MainWindow::on_pushButtonHelp_clicked()
{
    helpDialog* helpdial = new helpDialog(this);
    helpdial->exec();
    delete helpdial;
}
