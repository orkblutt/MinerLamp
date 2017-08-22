#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "minerprocess.h"
#include "helpdialog.h"
#include "nvidianvml.h"
#include "nvocdialog.h"
#include "leddialog.h"
#include "nanopoolapi.h"

#include <QDebug>
#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>
#include <QCloseEvent>
#include <QLibrary>
#include <QDir>

#define MINERPATH           "minerpath"
#define MINERARGS           "minerargs"
#define AUTORESTART         "autorestart"
#define MAX0MHS             "max0mhs"
#define RESTARTDELAY        "restartdelay"
#define ZEROMHSDELAY        "zeromhsdelay"
#define AUTOSTART           "autostart"
#define DISPLAYSHAREONLY    "shareonly"
#define DELAYNOHASH         "delaynohash"

#ifdef NVIDIA
#define NVIDIAOPTION        "nvidia_options"
#define NVLEDHASHINTENSITY  "nv_led_hash_intensity"
#define NVLEDSHAREINTENSITY "nv_led_share_intensity"
#define NVLEDBLINKON        "nv_led_blink_on"

#define NVOCOPTION          "nvidia_oc_options"

#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _isMinerRunning(false),
    _isStartStoping(false),
    _errorCount(0),
    _nano(Q_NULLPTR)
{

    _process = new MinerProcess();
    _settings = new QSettings(QString(QDir::currentPath() + QDir::separator() + "minerlamp.ini"), QSettings::IniFormat);

    ui->setupUi(this);

    _process->setLogControl(ui->textEdit);

    connect(_process, &MinerProcess::emitStarted, this, &MainWindow::onMinerStarted);
    connect(_process, &MinerProcess::emitStoped, this, &MainWindow::onMinerStoped);
    connect(_process, &MinerProcess::emitHashRate, this, &MainWindow::onHashrate);
    connect(_process, &MinerProcess::emitError, this, &MainWindow::onError);

#ifdef NVIDIA
    QLibrary lib("nvml.dll");
    if (!lib.load())
    {
        lib.setFileName("C:\\Program Files\\NVIDIA Corporation\\NVSMI\\nvml.dll");
        if(!lib.load())
        {
            ui->textEdit->append("Cannot find nvml.dll. NVAPI monitoring won't work.");
            ui->textEdit->append("Be sure to have the latest nvidia drivers.");
        }
    }
    else
    {

        _maxGPUTemp = new maxGPUThread(this);
        connect(_maxGPUTemp, &maxGPUThread::gpuInfoSignal, this, &MainWindow::onGPUInfo);

        _maxGPUTemp->start();
    }
#endif

    loadParameters();

    setupToolTips();


    createActions();

    createTrayIcon();

    setIcon();

    connect(_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);

    _trayIcon->show();

    setupEditor();

    ui->lcdNumberHashRate->display("0.00");
#ifndef NVIDIA
    ui->groupBoxNvidia->hide();
#endif

    if(ui->checkBoxAutoStart->isChecked())
    {
        _starter = new autoStart(this);
        connect(_starter, SIGNAL(readyToStartMiner()), this, SLOT(onReadyToStartMiner()));
        _starter->start();
    }

    ui->pushButtonShowHideLog->setChecked(true);
    ui->pushButtonPool->setChecked(false);
    ui->groupBoxPools->hide();


    int pos = ui->lineEditArgs->text().indexOf("-O ");
    if(pos > 0)
        ui->lineEditAccount->setText(ui->lineEditArgs->text().mid(pos + 3
                                                                  , ui->lineEditArgs->text().indexOf(" 0x") > 0 ? 42 : 40));
}

MainWindow::~MainWindow()
{
    saveParameters();

    _process->stop();

    delete _process;
    delete _settings;
    delete ui;
}

void MainWindow::loadParameters()
{
    ui->lineEditMinerPath->setText(_settings->value(MINERPATH).toString());
    ui->lineEditArgs->setText(_settings->value(MINERARGS).toString());
    ui->groupBoxWatchdog->setChecked(_settings->value(AUTORESTART).toBool());
    ui->spinBoxMax0MHs->setValue(_settings->value(MAX0MHS).toInt());
    ui->spinBoxDelay->setValue(_settings->value(RESTARTDELAY).toInt());
    ui->spinBoxDelay0MHs->setValue(_settings->value(ZEROMHSDELAY).toInt());
    ui->checkBoxAutoStart->setChecked(_settings->value(AUTOSTART).toBool());
    ui->checkBoxOnlyShare->setChecked(_settings->value(DISPLAYSHAREONLY).toBool());
    ui->spinBoxDelayNoHash->setValue(_settings->value(DELAYNOHASH).toInt());

    _process->setShareOnly(_settings->value(DISPLAYSHAREONLY).toBool());
    _process->setRestartOption(_settings->value(AUTORESTART).toBool());

}

void MainWindow::saveParameters()
{
    _settings->setValue(MINERPATH, ui->lineEditMinerPath->text());
    _settings->setValue(MINERARGS, ui->lineEditArgs->text());
    _settings->setValue(AUTORESTART, ui->groupBoxWatchdog->isChecked());
    _settings->setValue(MAX0MHS, ui->spinBoxMax0MHs->value());
    _settings->setValue(RESTARTDELAY, ui->spinBoxDelay->value());
    _settings->setValue(ZEROMHSDELAY, ui->spinBoxDelay0MHs->value());
    _settings->setValue(AUTOSTART, ui->checkBoxAutoStart->isChecked());
    _settings->setValue(DISPLAYSHAREONLY, ui->checkBoxOnlyShare->isChecked());
    _settings->setValue(DELAYNOHASH, ui->spinBoxDelayNoHash->value());
}

void MainWindow::setVisible(bool visible)
{
    _maximizeAction->setEnabled(!isMaximized());
    _restoreAction->setEnabled(isMaximized() || !visible);
    QMainWindow::setVisible(visible);
}

void MainWindow::startMiner()
{
    on_pushButton_clicked();
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
    QIcon icon(":/images/logo.png");
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

    _helpAction = new QAction(tr("&About"), this);
    connect(_helpAction, &QAction::triggered, this, &MainWindow::onHelp);

}

void MainWindow::createTrayIcon()
{
    _trayIconMenu = new QMenu(this);
    _trayIconMenu->addAction(_minimizeAction);
    _trayIconMenu->addAction(_maximizeAction);
    _trayIconMenu->addAction(_restoreAction);
    _trayIconMenu->addAction(_helpAction);
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

void MainWindow::setupToolTips()
{

    ui->lcdNumberHashRate->setToolTip("Displaing the current hashrate");
#ifdef NVIDIA
    ui->lcdNumberGPUCount->setToolTip("Number of nVidia GPU(s)");

    ui->lcdNumberMaxGPUTemp->setToolTip("Displaing the current higher temperature");
    ui->lcdNumberMinGPUTemp->setToolTip("Displaing the current lower temperature");

    ui->lcdNumberMaxFanSpeed->setToolTip("Displaing the current higher fan speed in percent of the max speed");
    ui->lcdNumberMinFanSpeed->setToolTip("Displaing the current lower fan speed in percent of the max speed");

    ui->lcdNumberMaxMemClock->setToolTip("Displaing the current higher memory clock");
    ui->lcdNumberMinMemClock->setToolTip("Displaing the current lower memory clock");

    ui->lcdNumberMaxGPUClock->setToolTip("The GPU in your rig with the higher clock");
    ui->lcdNumberMinGPUClock->setToolTip("The GPU in your rig with the lower clock");

    ui->lcdNumberMaxWatt->setToolTip("Displaing the current higher power draw in Watt");
    ui->lcdNumberMinWatt->setToolTip("Displaing the current lower power draw in Watt");

    ui->lcdNumberTotalPowerDraw->setToolTip("The total power used by the GPUs");

#endif
    if(!ui->groupBoxWatchdog->isChecked())
        ui->groupBoxWatchdog->setToolTip("Check it to activate the following watchdog options");
    else
        ui->groupBoxWatchdog->setToolTip("");

    ui->pushButtonOC->setToolTip("Not yet implemented... soon ;-)");
    ui->checkBoxBlinkLED->setToolTip("Blink baby! Blink!");

}


void MainWindow::on_pushButton_clicked()
{
    saveParameters();
    if(ui->lineEditMinerPath->text().isEmpty() || ui->lineEditArgs->text().isEmpty()) return;

    if(!_isStartStoping) // avoid to start/stop more than once on dbl clic
    {
        _isStartStoping = true;

        if(!_isMinerRunning)
        {
            _process->setMax0MHs(ui->spinBoxMax0MHs->value());
            _process->setRestartDelay(ui->spinBoxDelay->value());
            _process->setRestartOption(ui->groupBoxWatchdog->isChecked());
            _process->setDelayBeforeNoHash(ui->spinBoxDelayNoHash->value());
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
    ui->lcdNumberHashRate->setPalette(Qt::gray);
    ui->lcdNumberHashRate->display("0.00");

    _trayIcon->setToolTip(QString("Miner's lamp"));
}

void MainWindow::onHashrate(QString &hashrate)
{
    QString hrValue = hashrate.mid(0, hashrate.indexOf("MH/s"));

    this->setWindowTitle(QString("Miner's Lamp - " + hashrate + " - Restart count: " + QString::number(_errorCount)));
    if(hrValue.toDouble() == 0)
        ui->lcdNumberHashRate->setPalette(Qt::red);
    else
        ui->lcdNumberHashRate->setPalette(Qt::green);

    ui->lcdNumberHashRate->display(hrValue);

    _trayIcon->setToolTip(QString("Miner's lamp - " + hashrate));
}

void MainWindow::onError()
{
    _errorCount++;
    _trayIcon->showMessage("Miner's lamp report"
                           , "An error has been detected in ethminer.\n" + ui->groupBoxWatchdog->isChecked() ? "Miner's lamp restarted it automaticaly" : "Check the watchdog option checkbox if you want Miner's lamp to restart it on error");
}

const QColor MainWindow::getTempColor(unsigned int temp)
{
    if(temp < 50)
        return Qt::green;
    else if(temp < 65)
        return Qt::yellow;
    else if(temp < 72)
        return QColor("orange");

    return Qt::red;
}

void MainWindow::on_groupBoxWatchdog_clicked(bool checked)
{
    _process->setRestartOption(checked);
    if(checked)
        ui->groupBoxWatchdog->setToolTip("");
    else
        ui->groupBoxWatchdog->setToolTip("Check it to activate the following watchdog options");

}

void MainWindow::on_spinBoxMax0MHs_valueChanged(int arg1)
{
    _process->setMax0MHs(arg1);
}

void MainWindow::on_spinBoxDelay_valueChanged(int arg1)
{
    _process->setRestartDelay(arg1);
}

void MainWindow::on_spinBoxDelay0MHs_valueChanged(int arg1)
{
    _process->setDelayBefore0MHs(arg1);
}

void MainWindow::onReadyToStartMiner()
{
    on_pushButton_clicked();
}


void MainWindow::onHelp()
{
    on_pushButtonHelp_clicked();
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

void MainWindow::on_spinBoxDelayNoHash_valueChanged(int arg1)
{
    _process->setDelayBeforeNoHash(arg1);
}




autoStart::autoStart(QObject *pParent)
{
}

void autoStart::run()
{
    QThread::sleep(2);
    emit readyToStartMiner();
}

#ifdef NVIDIA

void MainWindow::onGPUInfo(unsigned int gpucount
                           , unsigned int maxgputemp
                           , unsigned int mingputemp
                           , unsigned int maxfanspeed
                           , unsigned int minfanspeed
                           , unsigned int maxmemclock
                           , unsigned int minmemclock
                           , unsigned int maxgpuclock
                           , unsigned int mingpuclock
                           , unsigned int maxpowerdraw
                           , unsigned int minpowerdraw
                           , unsigned int totalpowerdraw)
{

    ui->lcdNumberMaxGPUTemp->setPalette(getTempColor(maxgputemp));
    ui->lcdNumberMinGPUTemp->setPalette(getTempColor(mingputemp));

    ui->lcdNumberGPUCount->display((int)gpucount);

    ui->lcdNumberMaxGPUTemp->display((int)maxgputemp);
    ui->lcdNumberMinGPUTemp->display((int)mingputemp);

    ui->lcdNumberMaxFanSpeed->display((int)maxfanspeed);
    ui->lcdNumberMinFanSpeed->display((int)minfanspeed);

    ui->lcdNumberMaxMemClock->display((int)maxmemclock);
    ui->lcdNumberMinMemClock->display((int)minmemclock);

    ui->lcdNumberMaxGPUClock->display((int)maxgpuclock);
    ui->lcdNumberMinGPUClock->display((int)mingpuclock);

    ui->lcdNumberMaxWatt->display((double)maxpowerdraw / 1000);
    ui->lcdNumberMinWatt->display((double)minpowerdraw / 1000);

    ui->lcdNumberTotalPowerDraw->display((double)totalpowerdraw / 1000);

}


maxGPUThread::maxGPUThread(QObject *pParent)
{

}

void maxGPUThread::run()
{
    nvidiaNVML nvml;
    if(!nvml.initNVML()) return;

    while(1)
    {
        unsigned int gpucount = nvml.getGPUCount();

        unsigned int maxTemp = nvml.getHigherTemp();
        unsigned int minTemp = nvml.getLowerTemp();
        unsigned int maxfanspeed = nvml.getHigherFanSpeed();
        unsigned int minfanspeed = nvml.getLowerFanSpeed();
        unsigned int maxmemclock = nvml.getMemMaxClock();
        unsigned int minmemclock = nvml.getMemLowerClock();
        unsigned int maxgpuclock = nvml.getGPUMaxClock();
        unsigned int mingpuclock = nvml.getGPUMinClock();
        unsigned int maxpowerdraw = nvml.getMaxPowerDraw();
        unsigned int minpowerdraw = nvml.getMinPowerDraw();
        unsigned int totalpowerdraw = nvml.getPowerDrawSum();

        emit gpuInfoSignal(gpucount
                           , maxTemp
                           , minTemp
                           , maxfanspeed
                           , minfanspeed
                           , maxmemclock
                           , minmemclock
                           , maxgpuclock
                           , mingpuclock
                           , maxpowerdraw
                           , minpowerdraw
                           , totalpowerdraw);

        QThread::sleep(5);
    }

    nvml.shutDownNVML();
}

void MainWindow::on_pushButtonOC_clicked()
{
    nvOCDialog* dlg = new nvOCDialog(*_settings, this);
    dlg->exec();
    delete dlg;

}

void MainWindow::on_checkBoxBlinkLED_clicked(bool checked)
{
    unsigned short hash = 0 , share = 0;

    _settings->beginGroup(NVIDIAOPTION);

    if(checked)
    {
        hash = _settings->value(NVLEDHASHINTENSITY, hash).toInt();
        share = _settings->value(NVLEDSHAREINTENSITY, share).toInt();
        LEDDialog* led = new LEDDialog(hash, share, this);
        if(led)
        {
            if(led->exec())
            {
                led->getValues(hash, share);
                _process->setLEDOptions(hash, share, true);
            }
            else
                ui->checkBoxBlinkLED->setChecked(false);
            delete led;
        }
    }
    else
        _process->setLEDOptions(hash, share, true);

    _settings->setValue(NVLEDBLINKON, checked);

    if(checked)
    {
        _settings->setValue(NVLEDHASHINTENSITY, hash);
        _settings->setValue(NVLEDSHAREINTENSITY, share);
    }
    _settings->endGroup();
}

#endif

void MainWindow::on_pushButtonPool_clicked(bool checked)
{
    if(checked)
        ui->groupBoxPools->show();
    else
        ui->groupBoxPools->hide();
}

void MainWindow::on_pushButtonShowHideLog_clicked(bool checked)
{
    if(checked)
        ui->textEdit->show();
    else
    {
        QRect rect = ui->textEdit->geometry();
        ui->textEdit->hide();
        QRect winRect = geometry();
        resize(winRect.width(), winRect.height() - rect.height());
    }
}

void MainWindow::on_pushButtonDisplayPoolStats_clicked()
{

    if(!_nano)
    {
        _nano = new nanopoolAPI(ui->lineEditAccount->text(), this);
        connect(_nano, &nanopoolAPI::emitBalance, this, &MainWindow::onBalance);
        connect(_nano, &nanopoolAPI::emitUSerInfo, this, &MainWindow::onPoolUserInfo);

    }

    _nano->getUserInfo();
}

void MainWindow::onBalance(double balance)
{
    qDebug() << balance;
    ui->lcdNumberBalance->display(balance);
}

void MainWindow::onPoolUserInfo(double userBalance
                                , double currentCalcultatedHashRate
                                , double averageHashRate1H
                                , double averageHashRate3H
                                , double averageHashRate6H
                                , double averageHashRate12H
                                , double averageHashRate24H)
{
    ui->lcdNumberBalance->display(userBalance);
    ui->lcdNumberCalculatedHR->display(currentCalcultatedHashRate);
    ui->lcdNumberAvrgHr6H->display(averageHashRate6H);
}
