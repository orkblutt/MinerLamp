#include "minerprocess.h"
#include <QTextStream>
#include <QDebug>
#include <QRegExp>
#include <QThread>

MinerProcess::MinerProcess():
    _isRunning(false),
    _0mhs(0),
    _restartDelay(2),
    _autoRestart(true),
    _shareOnly(false)
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
        //qDebug() << "stdout:" << line;
        if(line.length() > 1 && !_shareOnly)
            _log->append(line.trimmed());
    }
}

void MinerProcess::onReadyToReadStderr()
{
    QString line(_miner.readAllStandardError());

    //qDebug() << "stderr:" << line;
    if(line.length() > 1)
    {
        QStringList list = line.split(QRegExp("\r\n"), QString::SkipEmptyParts);
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
            QString hasrate(list.at(7) + " " + list.at(8));

            emit emitHashRate(hasrate);

            if(list.at(7) == "0.00MH/s")
                _0mhs++;
            else
                _0mhs = 0;

            if(_0mhs > _max0mhs)
            {
                restart();
            }
        }
    }
}

void MinerProcess::onExit()
{
    _log->append("ethminer exit");
    _isRunning = false;
    _0mhs = 0;
    emit emitStoped();
}

void MinerProcess::onStarted()
{
    _log->append("ethminer start");
    _isRunning = true;
    _0mhs = 0;
    emit emitStarted();
}


void MinerProcess::start(const QString &path, const QString& args)
{
    _minerPath = path;
    _minerArgs = args;

    QStringList arglist = args.split(" ");
    _miner.start(path, arglist);
    _isRunning = true;
}

void MinerProcess::stop()
{
    _miner.kill();
    _miner.waitForFinished();
    _0mhs = 0;
    _isRunning = false;
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
