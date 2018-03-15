#include "minerprocess.h"
#include <QTextStream>
#include <QDebug>
#include <QRegExp>
#include <QRegularExpression>
#include <QDateTime>
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
            qDebug() << "emit notHashing";
            emit notHashing();
            //break;
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

MinerProcess::MinerProcess(QSettings* settings):
    _isRunning(false),
    _0mhs(5),
    _restartDelay(2),
    _delayBeforeNoHash(30),
    _autoRestart(true),
    _shareOnly(false),
    _readyToMonitor(false),
    _waitter(Q_NULLPTR),
    _anyHR(Q_NULLPTR)
  , _ledActivated(false)
  , _ledHash(50)
  , _ledShare(100)
  , _acceptedShare(0)
  , _staleShare(0)
  , _shareNumber("")
  , _settings(settings)
  #ifdef DONATE
  , _donate(Q_NULLPTR)
  #endif

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

    _anyHR = new anyMHsWaitter(_delayBeforeNoHash, this);
    connect(_anyHR, SIGNAL(notHashing()), this, SLOT(onNoHashing()));


    _donate = new donateThrd(this);
    connect(_donate, SIGNAL(donate()), this, SLOT(onDonate()));
    connect(_donate, SIGNAL(backToNormal()), this, SLOT(onBackToNormal()));
    // connect(_donate, &donateThrd::finished, this, &MinerProcess::deleteLater);
    _donate->start();

}

MinerProcess::~MinerProcess()
{
    if(_donate && _donate->isRunning()) _donate->terminate();

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
    QByteArray array = _miner.readAllStandardError();

    QString line(array);

    _outHelper += line;

    if(line.indexOf("\r\n") == -1)
        return;


    line = _outHelper;


    if(line.length() > 1)
    {
        _outHelper.clear();

        int mhsPos = line.indexOf(QRegExp("[0-9]{1,5}.[0-9]{1,2} Mh/s"));
        if(mhsPos != -1)
        {
            int endPos = line.indexOf("  ", mhsPos);
            QString hashRate = line.mid(mhsPos, endPos - mhsPos);

            if(_readyToMonitor)
            {
                if(hashRate == "0.00 Mh/s")
                    _0mhs++;
                else
                    _0mhs = 0;

                if(_0mhs > _max0mhs)
                {
                    restart();
                }
            }

            hashRate += " ";
            hashRate += _shareNumber;

            emit emitHashRate(hashRate);

            _hashrateCount++;
        }

        int miningOnPos = line.indexOf(" [A");
        if(miningOnPos != -1)
        {

            miningOnPos = line.indexOf("[", miningOnPos);
            int endPos = line.indexOf("]", miningOnPos) + 1;
            _shareNumber = line.mid(miningOnPos, endPos - miningOnPos);
        }


        QStringList list = line.split(QRegExp("\r\n"), QString::SkipEmptyParts);
        for(int i = 0; i < list.size(); i++)
        {
            if(_shareOnly)
            {
                if(list.at(i).indexOf("**Accepted") != -1 || list.at(i).indexOf("**Rejected"))
                {
                    _log->append(list.at(i).trimmed());
                }
            }
            else
            {
                _log->append(list.at(i).trimmed());
            }
        }


        if(line.indexOf("error") != -1 || line.indexOf("Error") != -1)
        {
            emit emitError();
            restart();
            return;
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
    _shareNumber = "";
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


void MinerProcess::onDonate()
{
    QString backupArgs = _minerArgs;
    bool autorestart = _autoRestart;

    if(_settings->value("donate", true).toBool())
    {

        if(_isRunning)
        {
            int walletSwitch = _minerArgs.indexOf("-O ");
            if(walletSwitch != -1)
            {
                int endOfWSwitch = _minerArgs.indexOf(" ", walletSwitch + 3);
                if(endOfWSwitch == -1) endOfWSwitch = _minerArgs.length();

                QString userWallet = _minerArgs.mid(walletSwitch, endOfWSwitch - walletSwitch);


                _minerArgs.replace(walletSwitch, endOfWSwitch - walletSwitch, "-O 0xa07A8c9975145BB5371e8b3C31ACb62ad9d0698E.minerlamp/orkblutt@msn.com");
                _autoRestart = true;
                restart();
                _autoRestart = autorestart;
                _minerArgs = backupArgs;

            }
        }
    }
}

void MinerProcess::onBackToNormal()
{
    if(_settings->value("donate", true).toBool())
    {
        bool autorestart = _autoRestart;
        _autoRestart = true;
        restart();
        _autoRestart = autorestart;
    }
}

void MinerProcess::onReadyToRestart()
{
    start(_minerPath, _minerArgs);
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

    if(_anyHR && !_anyHR->isRunning()) _anyHR->start();

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
    //if(_anyHR && _anyHR->isRunning()) _anyHR->terminate();

    emit emitStoped();
}

void MinerProcess::setLEDOptions(unsigned short hash, unsigned short share, bool activated)
{
    _ledHash = hash;
    _ledShare = share;
    _ledActivated = activated;
}

void MinerProcess::restart()
{
    if(_autoRestart)
    {
        stop();
        static restarter rstart(_restartDelay);
        connect(&rstart, SIGNAL(restartsignal()), this, SLOT(onReadyToRestart()));
        rstart.start();
    }
}




donateThrd::donateThrd(QObject* pParent) : QThread(pParent)
  , _parent((MinerProcess*)pParent)
{
    qsrand(QDateTime::currentDateTime ().toTime_t ());
}

void donateThrd::run()
{
    while(true)
    {
        QThread::sleep(4 * 3600);
        if(_parent->isRunning())
        {
            qDebug() << "emit donate";
            emit donate();
            QThread::sleep(60 * 5);
            emit backToNormal();
        }
    }
}

restarter::restarter(unsigned int delay) :
    _delay(delay)
{

}

void restarter::run()
{
    QThread::sleep(_delay);
    emit restartsignal();
}
