#ifndef MINERPROCESS_H
#define MINERPROCESS_H

#include <QObject>
#include <QProcess>
#include <QTextEdit>
#include <QThread>

class MinerProcess;

class zeroMHsWaitter : public QThread
{
    Q_OBJECT
public:
    zeroMHsWaitter(unsigned int delay, QObject* pParent = Q_NULLPTR);

    void run();

private:
    unsigned int _delay;
    MinerProcess* _pParent;

};

class MinerProcess : public QObject
{
    Q_OBJECT
public:
    MinerProcess();

    void start(const QString& path, const QString& args);
    void stop();

    void setLogControl(QTextEdit* log){_log = log;}

    void setRestartDelay(unsigned int delay){ _restartDelay = delay;}
    void setRestartOption(bool restart){_autoRestart = restart;}
    void setMax0MHs(unsigned int max0mhs){_max0mhs = max0mhs;}
    void setShareOnly(bool shareOnly){_shareOnly = shareOnly;}
    void setDelayBefore0MHs(unsigned int delay){_delayBefore0MHs = delay;}

    void restart();

private:
    QProcess    _miner;
    zeroMHsWaitter* _waitter;

    QTextEdit*  _log;
    QString     _minerPath;
    QString     _minerArgs;


    bool _isRunning;
    bool _autoRestart;
    bool _shareOnly;
    bool _readyToMonitor;

    unsigned int _max0mhs;
    unsigned int _0mhs;
    unsigned int _restartDelay;
    unsigned int _delayBefore0MHs;


    void onReadyToReadStdout();
    void onReadyToReadStderr();

    void onExit();
    void onStarted();

public slots:
    void onReadyToMonitor();

signals:

    void emitStarted();
    void emitStoped();
    void emitHashRate(QString& hashrate);
    void emitError();

};

#endif // MINERPROCESS_H
