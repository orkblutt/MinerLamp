#include "nanopoolapi.h"
#include <QNetworkCookie>

nanopoolAPI::nanopoolAPI(QString account, QObject *parent) : QObject(parent)
  , _accountAddress(account)
  , _balanceReply(Q_NULLPTR)
  , _hashrateReply(Q_NULLPTR)
  , _userinfoReply(Q_NULLPTR)
{
    _networkManager = new QNetworkAccessManager(this);
    connect(_networkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

}

void nanopoolAPI::getUserInfo()
{
    QString request = QString("https://api.nanopool.org/v1/eth/user/" + _accountAddress);
    QNetworkRequest netReq;
    netReq.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    netReq.setUrl(QUrl(request));
    _balanceReply = _networkManager->get(netReq);
}

void nanopoolAPI::getBalance()
{
    QString request = QString("https://api.nanopool.org/v1/eth/balance/" + _accountAddress);
    QNetworkRequest netReq;
    netReq.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    netReq.setUrl(QUrl(request));
    _balanceReply = _networkManager->get(netReq);
}

void nanopoolAPI::getHashrate()
{
    QString request = QString("https://api.nanopool.org/v1/eth/avghashrate/" + _accountAddress);
    QNetworkRequest netReq;
    netReq.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    netReq.setUrl(QUrl(request));
    _hashrateReply = _networkManager->get(netReq);
}

void nanopoolAPI::replyFinished(QNetworkReply *reply)
{
    if(reply == _balanceReply)
    {
        qDebug() << "balance";
    }
    else if(reply == _hashrateReply)
    {
        qDebug() << "hashrate";
    }


    QByteArray data = reply->readAll();

    qDebug() << data;

    reply->deleteLater();

}
