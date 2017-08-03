#ifndef NANOPOOLAPI_H
#define NANOPOOLAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>


class workerObject : public QObject
{
    Q_OBJECT
public:
    workerObject();

    void setName(QString& name);
    void setAvrgHR(double& hashrate);
    void setShareNumber(unsigned int share);

    const QString& getName();
    const double& getAvrgHR();
    const unsigned int& getShareNumber();

private:
    QString         _workerName;
    double          _averageHashRate;
    unsigned int    _shareNumber;

};

class userAccount : public QObject
{
    Q_OBJECT
public:
    userAccount(QString account, QObject* parent);

protected:

    QString _accountAddress;
    double _currentCalcultatedHashRate;
    double _reportedHashRate;
    double _averageHashRate6H;
    double _averageHashRate12H;
    double _averageHashRate24H;

    double _userBalance;
    double _totalPayment;

    QList<workerObject*> _wokerList;
};

class nanopoolAPI : public userAccount
{
    Q_OBJECT
public:
    nanopoolAPI(QString account, QObject* parent);

    void getUserInfo();
    void getBalance();
    void getHashrate();

    void setAccount(QString& account);

private:

    QNetworkAccessManager* _networkManager;

    QNetworkReply* _userinfoReply;
    QNetworkReply* _balanceReply;
    QNetworkReply* _hashrateReply;

public slots:
    void replyFinished(QNetworkReply* reply);

signals:

    void emitBalance(double balance);

};

#endif // NANOPOOLAPI_H
