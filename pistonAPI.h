#ifndef PISTONAPI_H
#define PISTONAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>

class PistonExecutor : public QObject {
    Q_OBJECT
public:
    explicit PistonExecutor(QObject *parent = nullptr);
    void fetchRuntimes();
    void executeCode(const QString &language, const QString &code);

signals:
    void runtimesFetched(const QJsonArray &runtimes);
    void executionFinished(const QString &result);
    void errorOccurred(const QString &error);

private:
    QNetworkAccessManager *manager;
};

#endif // PISTONAPI_H
