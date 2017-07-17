#ifndef MINERPROCESS_H
#define MINERPROCESS_H

#include <QObject>
#include <QProcess>
#include <QTextEdit>

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

    void restart();

private:
    QProcess _miner;
    QTextEdit* _log;
    QString _minerPath;
    QString _minerArgs;

    bool _isRunning;
    bool _autoRestart;
    unsigned int _max0mhs;
    unsigned int _0mhs;
    unsigned int _restartDelay;
    bool _shareOnly;

    void onReadyToReadStdout();
    void onReadyToReadStderr();

    void onExit();
    void onStarted();

signals:

    void emitStarted();
    void emitStoped();
    void emitHashRate(QString& hashrate);
    void emitError();



};

#endif // MINERPROCESS_H
