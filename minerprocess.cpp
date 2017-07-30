#include "minerprocess.h"
#include <QTextStream>q
#include <QDebug>
#include <QRegExp>
#include <QThread>

anyMHsWaitter::anyMHsWaitter(unsigned int delay, QObject *pParent) : QThread(pParent)
  , _pParent((MinerProcess*)pParent)
  , _delay(delay)
  , _hashrateCount(0)
{

}


void anyMHsWaitter::run()
{
    while(true)
    {
        QThread::sleep(_delay);
        if(_hashrateCount == _pParent->getCurrentHRCount())
        {
            emit notHashing();
            break;
        }
        _hashrateCount = _pParent->getCurrentHRCount();
    }
}



zeroMHsWaitter::zeroMHsWaitter(unsigned int delay, QObject* pParent /*= Q_NULLPTR*/) : QThread(pParent)
  , _pParent((MinerProcess*)pParent)
  , _delay(delay)
{
}


void zeroMHsWaitter::run()
{
    QThread::sleep(_delay);
}

MinerProcess::MinerProcess():
    _isRunning(false),
    _0mhs(5),
    _restartDelay(2),
    _delayBeforeNoHash(30),
    _autoRestart(true),
    _shareOnly(false),
    _readyToMonitor(false),
    _waitter(Q_NULLPTR),
    _anyHR(Q_NULLPTR)
{

    connect(&_miner, &QProcess::readyReadStandardOutput,
            this, &MinerProcess::onReadyToReadStdout);

    connect(&_miner, &QProcess::readyReadStandardError,
            this, &MinerProcess::onReadyToReadStderr);

    connect(&_miner, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this, &MinerProcess::onExit);

    connect (&_miner, &QProcess::started,
             this, &MinerProcess::onStarted);

    _miner.setReadChannel(QProcess::StandardOutput);
}

void MinerProcess::onReadyToReadStdout()
{
    QTextStream stream(&_miner);
    while (!stream.atEnd())
    {
        QString line = stream.readLine();
        if(line.length() > 1 && !_shareOnly)
            _log->append(line.trimmed());
    }
}

void MinerProcess::onReadyToReadStderr()
{
    QString line(_miner.readAllStandardError());

    if(line.length() > 1)
    {
        QStringList list = line.split(QRegExp("\r\n"), QString::SkipEmptyParts);
        qDebug() << list;
        for(int i = 0; i < list.size(); i++)
        {
            if(_shareOnly)
            {
                if(list.at(i).indexOf("Solution") != -1
                        || list.at(i).indexOf("Nonce") != -1
                        || list.at(i).indexOf("Submitted") != -1)
                {
                    _log->append(list.at(i).simplified());
                }
            }
            else
            {
                _log->append(list.at(i).simplified());
            }
        }

        if(line.indexOf("error") != -1 || line.indexOf("Error") != -1)
        {
            emit emitError();
            restart();
            return;
        }

        if(line.indexOf("MH/s") != -1)
        {
            QRegExp regex("[, ]");
            QStringList list = line.split(regex, QString::SkipEmptyParts);
            QString hasrate(list.at(6) + " " + list.at(7));

            emit emitHashRate(hasrate);

            if(_readyToMonitor)
            {
                if(list.at(7) == "0.00MH/s")
                    _0mhs++;
                else
                    _0mhs = 0;

                if(_0mhs > _max0mhs)
                {
                    restart();
                }
            }
            _hashrateCount++;
        }
    }
}

void MinerProcess::onExit()
{
    _log->append("ethminer exit");
    _isRunning = false;
    _0mhs = 0;
    if(_waitter && _waitter->isRunning()) _waitter->terminate();
    if(_anyHR && _anyHR->isRunning()) _anyHR->terminate();

    emit emitStoped();
}

void MinerProcess::onStarted()
{
    _log->append("ethminer start");
    _isRunning = true;
    _0mhs = 0;
    emit emitStarted();
}

void MinerProcess::onReadyToMonitor()
{
    _readyToMonitor = true;
}

void MinerProcess::onNoHashing()
{
    emit emitError();
    restart();
}


void MinerProcess::start(const QString &path, const QString& args)
{
    _minerPath = path;
    _minerArgs = args;

    QStringList arglist = args.split(" ");

    if(_delayBefore0MHs > 0)
    {
        _readyToMonitor = false;
        if(_waitter && _waitter->isRunning()) _waitter->terminate();
        if(_waitter) delete _waitter;

        _waitter = new zeroMHsWaitter(_delayBefore0MHs, this);

        connect(_waitter, SIGNAL(finished()), this, SLOT(onReadyToMonitor()), Qt::DirectConnection);

        _waitter->start();
    }
    else
        _readyToMonitor = true;

    _hashrateCount = 0;

    if(_anyHR && _anyHR->isRunning())
    {
        _anyHR->terminate();
        delete _anyHR;

    }

    _anyHR = new anyMHsWaitter(_delayBeforeNoHash, this);
    connect(_anyHR, SIGNAL(notHashing()), this, SLOT(onNoHashing()), Qt::DirectConnection);
    _anyHR->start();

    _miner.start(path, arglist);

    _isRunning = true;
}

void MinerProcess::stop()
{
    _miner.kill();
    _miner.waitForFinished();
    _0mhs = 0;
    _isRunning = false;
    if(_waitter && _waitter->isRunning()) _waitter->terminate();
    if(_anyHR && _anyHR->isRunning()) _anyHR->terminate();

    emit emitStoped();
}

void MinerProcess::restart()
{
    if(_autoRestart)
    {
        stop();
        QThread::sleep(_restartDelay);
        start(_minerPath, _minerArgs);
    }
}
