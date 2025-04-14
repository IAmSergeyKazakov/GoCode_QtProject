


/*PistonExecutor::PistonExecutor(QObject *parent) : QObject(parent) {
    manager = new QNetworkAccessManager(this);
}

void PistonExecutor::fetchRuntimes() {
    QUrl url("https://emkc.org/api/v2/piston/runtimes");
    QNetworkRequest request(url);

    QNetworkReply *reply = manager->get(request);
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            emit runtimesFetched(doc.array());
        } else {
            emit errorOccurred(reply->errorString());
        }
        reply->deleteLater();
    });
}

void PistonExecutor::executeCode(const QString &language, const QString &code) {
    QUrl url("https://emkc.org/api/v2/piston/execute");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject body;
    body["language"] = language;
    body["source"] = code;

    QNetworkReply *reply = manager->post(request, QJsonDocument(body).toJson());
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            emit executionFinished(doc.object()["output"].toString());
        } else {
            emit errorOccurred(reply->errorString());
        }
        reply->deleteLater();
    });
}*/


