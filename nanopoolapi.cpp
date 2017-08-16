#include "nanopoolapi.h"
#include <QNetworkCookie>



nanopoolAPI::nanopoolAPI(QString account, QObject *parent) : userAccount(account, parent)
  , _balanceReply(Q_NULLPTR)
  , _hashrateReply(Q_NULLPTR)
  , _userinfoReply(Q_NULLPTR)
{
    _networkManager = new QNetworkAccessManager(this);
    connect(_networkManager, &QNetworkAccessManager::finished,
            this, &nanopoolAPI::replyFinished, Qt::DirectConnection);

    qDebug() << account;
}

void nanopoolAPI::getUserInfo()
{
    QString request = QString("https://api.nanopool.org/v1/eth/user/" + _accountAddress);
    QNetworkRequest netReq;
    netReq.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    netReq.setUrl(QUrl(request));
    _userinfoReply = _networkManager->get(netReq);
}

void nanopoolAPI::getBalance()
{
    QString request = QString("https://api.nanopool.org/v1/eth/balance/" + _accountAddress);
    QNetworkRequest netReq;
    netReq.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    netReq.setUrl(QUrl(request));
    _balanceReply = _networkManager->get(netReq);
    qDebug() << _balanceReply;
}

void nanopoolAPI::getHashrate()
{
    QString request = QString("https://api.nanopool.org/v1/eth/avghashrate/" + _accountAddress);
    QNetworkRequest netReq;
    netReq.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    netReq.setUrl(QUrl(request));
    _hashrateReply = _networkManager->get(netReq);
}

void nanopoolAPI::setAccount(QString &account)
{
    _accountAddress = account;
}

void nanopoolAPI::replyFinished(QNetworkReply *reply)
{
    QByteArray data = reply->readAll();
    if(!data.isEmpty())
    {
        QJsonObject json = QJsonDocument::fromJson(data).object();

        if(reply == _balanceReply)
        {
            _userBalance = json.value("data").toDouble();
            qDebug() << "balance: " << _userBalance;

            emit emitBalance(_userBalance);
        }
        else if(reply == _hashrateReply)
        {
            qDebug() << "hashrate";
        }

        else if(reply == _userinfoReply)
        {

            QJsonObject jsonData = json.value("data").toObject();

            _userBalance = jsonData.value("balance").toString().toDouble();
            _currentCalcultatedHashRate = jsonData.value("hashrate").toString().toDouble();

            QJsonObject jsonHR = jsonData.value("avgHashrate").toObject();

            _averageHashRate1H = jsonHR["h1"].toString().toDouble();
            _averageHashRate3H = jsonHR["h3"].toString().toDouble();
            _averageHashRate6H = jsonHR["h6"].toString().toDouble();
            _averageHashRate12H = jsonHR["h12"].toString().toDouble();
            _averageHashRate24H = jsonHR["h24"].toString().toDouble();

            emit emitUSerInfo(_userBalance
                              , _currentCalcultatedHashRate
                              , _averageHashRate1H
                              , _averageHashRate3H
                              , _averageHashRate6H
                              , _averageHashRate12H
                              , _averageHashRate24H
                              );
        }

        qDebug() << data;
    }
    reply->deleteLater();
}




userAccount::userAccount(QString account, QObject* parent) :
    _accountAddress(account)
{

}
