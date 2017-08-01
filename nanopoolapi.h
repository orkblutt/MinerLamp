#ifndef NANOPOOLAPI_H
#define NANOPOOLAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

class nanopoolAPI : public QObject
{
    Q_OBJECT
public:
    nanopoolAPI(QString account, QObject* parent);

    void getUserInfo();
    void getBalance();
    void getHashrate();

private:
    QString _accountAddress;

    QNetworkAccessManager* _networkManager;

    QNetworkReply* _userinfoReply;
    QNetworkReply* _balanceReply;
    QNetworkReply* _hashrateReply;

public slots:
    void replyFinished(QNetworkReply* reply);

};

#endif // NANOPOOLAPI_H
