#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QThread>

#include "minerprocess.h"
#include "highlighter.h"


namespace Ui {
class MainWindow;
}

class autoStart : public QThread
{
    Q_OBJECT
public:
    autoStart(QObject* pParent = Q_NULLPTR);

    void run();

signals:

    void readyToStartMiner();
};

class maxGPUThread : public QThread
{
    Q_OBJECT
public:
    maxGPUThread(QObject* pParent = Q_NULLPTR);

    void run();

signals:

    void gpuInfoSignal(unsigned int gpucount
                       , unsigned int maxgputemp
                       , unsigned int mingputemp
                       , unsigned int maxfanspeed
                       , unsigned int minfanspeed
                       , unsigned int maxmemclock
                       , unsigned int minmemclock
                       , unsigned int maxpowerdraw
                       , unsigned int minpowerdraw);
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setVisible(bool visible) Q_DECL_OVERRIDE;

    void startMiner();

protected:
     void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private slots:
     void setIcon();
     void iconActivated(QSystemTrayIcon::ActivationReason reason);

private:
     void createActions();
     void createTrayIcon();

     void setupEditor();

     void setupToolTips();

     void loadParameters();
     void saveParameters();

private slots:
    void on_pushButton_clicked();

    void on_spinBoxMax0MHs_valueChanged(int arg1);

    void on_spinBoxDelay_valueChanged(int arg1);

    void on_checkBoxOnlyShare_clicked(bool checked);

    void on_pushButtonHelp_clicked();

    void on_spinBoxDelay0MHs_valueChanged(int arg1);

    void onReadyToStartMiner();

    void onGPUInfo(unsigned int gpucount
                   , unsigned int maxgputemp
                   , unsigned int mingputemp
                   , unsigned int maxfanspeed
                   , unsigned int minfanspeed
                   , unsigned int maxmemclock
                   , unsigned int minmemclock
                   , unsigned int maxpowerdraw
                   , unsigned int minpowerdraw);

    void onHelp();

    void on_groupBoxWatchdog_clicked(bool checked);

    void on_pushButtonOC_clicked();

    void on_spinBoxDelayNoHash_valueChanged(int arg1);

private:

    void onMinerStarted();
    void onMinerStoped();
    void onHashrate(QString& hashrate);
    void onError();

    const QColor getTempColor(unsigned int temp);

    Ui::MainWindow *ui;
    MinerProcess* _process;
    QSettings*  _settings;
    QIcon*       _icon;

    bool _isMinerRunning;
    bool _isStartStoping;

    unsigned int _errorCount;

    QSystemTrayIcon* _trayIcon;
    QMenu* _trayIconMenu;

    QAction* _minimizeAction;
    QAction* _maximizeAction;
    QAction* _restoreAction;
    QAction* _quitAction;
    QAction* _helpAction;

    Highlighter* _highlighter;


    //QThreads
    autoStart* _starter;
    maxGPUThread* _maxGPUTemp;
};

#endif // MAINWINDOW_H
