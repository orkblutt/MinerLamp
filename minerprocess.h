#ifndef MINERPROCESS_H
#define MINERPROCESS_H

#include <QObject>
#include <QProcess>
#include <QTextEdit>
#include <QThread>

#include "nvidiaapi.h"

class MinerProcess;
class donateThrd;

class blinkerLED : public QThread
{
    Q_OBJECT
public:
    blinkerLED(unsigned short hash, unsigned short share, QObject* pParent = Q_NULLPTR);

    void run();
private:
    unsigned short _hash;
    unsigned short _share;
    MinerProcess* _pParent;
};

// waitter before to monitor for O.OOMH/s
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

// Timer for any MH/s
// If no hasrate reported after a given delay
// this worker will signal for issue
class anyMHsWaitter : public QThread
{
    Q_OBJECT
public:
    anyMHsWaitter(unsigned int delay, QObject* pParent = Q_NULLPTR);

    void run();

private:
    unsigned int _delay;
    MinerProcess* _pParent;

    unsigned int _hashrateCount;

signals:
    void notHashing();

};

class MinerProcess : public QObject
{
    Q_OBJECT
public:
    MinerProcess();
    ~MinerProcess();

    void start(const QString& path, const QString& args);
    void stop();

    void setLogControl(QTextEdit* log){_log = log;}

    void setRestartDelay(unsigned int delay){ _restartDelay = delay;}
    void setRestartOption(bool restart){_autoRestart = restart;}
    void setMax0MHs(unsigned int max0mhs){_max0mhs = max0mhs;}
    void setShareOnly(bool shareOnly){_shareOnly = shareOnly;}
    void setDelayBefore0MHs(unsigned int delay){_delayBefore0MHs = delay;}
    void setDelayBeforeNoHash(unsigned int delay){_delayBeforeNoHash = delay;}

    unsigned int getCurrentHRCount(){return _hashrateCount;}

    void setLEDOptions(unsigned short hash, unsigned short share, bool activated);

    void restart();

    bool isRunning(){return _isRunning;}

#ifdef NVIDIA
    nvidiaAPI* getNVAPI(){return _nvapi;}
#endif

private:
    QProcess    _miner;
    zeroMHsWaitter* _waitter;
    anyMHsWaitter*  _anyHR;
    blinkerLED* _blinker;
#ifdef DONATE
    donateThrd* _donate;
#endif

#ifdef NVIDIA
    nvidiaAPI*  _nvapi;
#endif

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
    unsigned int _delayBeforeNoHash;

    unsigned int _hashrateCount;

    unsigned int _acceptedShare;
    unsigned int _staleShare;

    QString _shareNumber;

    unsigned short _ledHash;
    unsigned short _ledShare;

    bool _ledActivated;


    void onReadyToReadStdout();
    void onReadyToReadStderr();

    void onExit();
    void onStarted();

public slots:
    void onReadyToMonitor();
    void onNoHashing();

#ifdef DONATE
    void onDonate();
    void onBackToNormal();
#endif

signals:

    void emitStarted();
    void emitStoped();
    void emitHashRate(QString& hashrate);
    void emitError();

};


#ifdef DONATE
class donateThrd : public QThread
{
    Q_OBJECT

public:
    donateThrd(QObject* pParent = Q_NULLPTR);

    void run();

signals:
    void donate();
    void backToNormal();
private:
    MinerProcess* _parent;
};
#endif

#endif // MINERPROCESS_H
