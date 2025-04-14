#include <mainwindow.h>
#include <ui_mainwindow.h>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <unordered_map>
#include <QTimer>
#include <QThread>
#include <QElapsedTimer>
#include <mainPage.cpp>


bool isRunButtonClicked = false;
const QString pistonAPI_URL = "https://emkc.org/api/v2/piston/execute";
const QString userFileName = "code";
QString input, expectedOutput, userOutput, code;

QVector<QPair<QString, QString>> MainWindow::getProblemTestCases(bool isSubmit)
{
    QSqlQuery sqlQuery(db);
    QString query = "SELECT input, output FROM GC_testCases WHERE problem_id = ?";
    sqlQuery.prepare(query);
    sqlQuery.addBindValue(problemID);

    if (!sqlQuery.exec()) {
        loadingWindow.close();
        QMessageBox::critical(this, "Ошибка", "Не удалось получить тестовые данные");
    }

    QVector<QPair<QString, QString>> testCases;

    if(isSubmit){
        while (sqlQuery.next()) {
            testCases.append(qMakePair(
                sqlQuery.value("input").toString(),
                sqlQuery.value("output").toString()
                ));
        }
    }
    else{
        while (sqlQuery.next() && testCases.size() != 3) {
            testCases.append(qMakePair(
                sqlQuery.value("input").toString(),
                sqlQuery.value("output").toString()
                ));
        }
    }

    return testCases;
}


void MainWindow::callPistonApi(QVector<QPair<QString, QString>>& testCases, QString& code, QString& version, QString& normilizeLanguage)
{
    setLoadingWindow();
    auto completedTests = QSharedPointer<int>::create(0);
    auto allTestsPassed = QSharedPointer<bool>::create(true);
    auto errorReported = QSharedPointer<bool>::create(false);
    int totalTests = testCases.size();

    QString query = "SELECT time_limit FROM GC_problems WHERE id = :problemID";
    QString timeLimit = getValueFromDB(query, {problemID});
    auto output = QSharedPointer<QString>::create();
    auto executionTime = QSharedPointer<int>::create(0);
    auto passedTestCasesQuantity = QSharedPointer<int>::create(0);
    auto executionStatus = QSharedPointer<QString>::create();
    auto isExecutionFinished = QSharedPointer<bool>::create(false);

    static QVector<int> latencyMeasurements;
    static int errorCount = 0;
    const int MIN_LATENCY = 300;
    const int MAX_LATENCY = 2000;
    const int MAX_ERRORS = 3;

    query = "INSERT INTO GC_submissions (user_id, problem_id, verdict, passedTestCasesQuantity, totalTestCasesQuantity, code, creation_date) VALUES "
            "(?, ?, ?, ?, ?, ?, ?)";

    for (const auto& testCase : testCases)
    {
        if(*errorReported || *isExecutionFinished) {
            break;
        }

        int currentLatency = measureNetworkLatency(normilizeLanguage, version);
        currentLatency = qBound(MIN_LATENCY, currentLatency, MAX_LATENCY);

        latencyMeasurements.append(currentLatency);
        if(latencyMeasurements.size() > 5) {
            latencyMeasurements.removeFirst();
        }

        std::sort(latencyMeasurements.begin(), latencyMeasurements.end());
        int medianLatency = latencyMeasurements.empty() ? MIN_LATENCY :
                           latencyMeasurements[latencyMeasurements.size()/2];

        QThread::msleep(500);
        QElapsedTimer executionTimer;
        executionTimer.start();

        QJsonObject fileObject;
        fileObject["name"] = userFileName;
        fileObject["content"] = code;

        QJsonArray filesArray;
        filesArray.append(fileObject);

        QJsonObject requestBody;
        requestBody["language"] = normilizeLanguage;
        requestBody["version"] = version;
        requestBody["files"] = filesArray;
        requestBody["stdin"] = testCase.first;

        QNetworkRequest request(QUrl("https://emkc.org/api/v2/piston/execute"));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QNetworkReply* reply = networkManager->post(request, QJsonDocument(requestBody).toJson());

        connect(reply, &QNetworkReply::finished, this, [=]() {
            if(*isExecutionFinished) {
                reply->deleteLater();
                return;
            }

            int totalElapsed = executionTimer.elapsed();
            int adjustedLatency = qMax(medianLatency, currentLatency);
            *executionTime = qMax(0, totalElapsed - adjustedLatency) - 500;
            if(*executionTime <= 0)
                *executionTime = 0;

            if (reply->error() == QNetworkReply::ProtocolUnknownError) {
                *isExecutionFinished = true;
                *allTestsPassed = false;
                errorCount = qMin(errorCount + 1, MAX_ERRORS);
                callErrorBox(ERROR_TITLE, "Сервер перегружен. Пожалуйста, попробуйте позже.");
                reply->deleteLater();
                return;
            }
            else if (reply->error() != QNetworkReply::NoError) {
                *isExecutionFinished = true;
                *allTestsPassed = false;
                *errorReported = true;
                callErrorBox(ERROR_TITLE, "Ошибка сети: " + reply->errorString());
                reply->deleteLater();
                return;
            }

            QJsonParseError parseError;
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &parseError);
            reply->deleteLater();

            if (parseError.error != QJsonParseError::NoError) {
                *isExecutionFinished = true;
                *allTestsPassed = false;
                *errorReported = true;
                callErrorBox(ERROR_TITLE, "Ошибка парсинга ответа");
                return;
            }

            QJsonObject response = doc.object();
            QJsonObject run = response["run"].toObject();
            *output = run["output"].toString().trimmed();
            QString errorOutput = run["stderr"].toString();
            bool isTimeLimit = *executionTime >= timeLimit.toInt();

            if (!errorOutput.isEmpty()) {
                *isExecutionFinished = true;
                *allTestsPassed = false;
                *errorReported = true;
                *executionStatus = "RE";
                setProblemExecutionResults(RE_problemVerdict, testCase, *executionTime, *output,
                                       QString::number(*passedTestCasesQuantity), QString::number(testCases.size()));
            }
            else if(isTimeLimit) {
                *isExecutionFinished = true;
                *allTestsPassed = false;
                *errorReported = true;
                *executionStatus = "TL";
                setProblemExecutionResults(TL_problemVerdict, testCase, *executionTime, *output,
                                       QString::number(*passedTestCasesQuantity), QString::number(testCases.size()));
            }
            else if (*output != testCase.second.trimmed()) {
                *isExecutionFinished = true;
                *allTestsPassed = false;
                *errorReported = true;
                *executionStatus = "WA";
                setProblemExecutionResults(WA_problemVerdict, testCase, *executionTime, *output,
                                       QString::number(*passedTestCasesQuantity), QString::number(testCases.size()));
            }
            else {
                *passedTestCasesQuantity += 1;
            }

            (*completedTests)++;
            if (*completedTests >= totalTests || *isExecutionFinished) {
                loadingWindow.close();
                if (*allTestsPassed) {
                    *executionStatus = "OK";
                    setProblemExecutionResults(OK_problemVerdict, testCase, *executionTime, *output,
                                           QString::number(*passedTestCasesQuantity), QString::number(testCases.size()));
                }

                QVector<QString> executionInfo = {
                    global_userID,
                    problemID,
                    *executionStatus,
                    QString::number(*passedTestCasesQuantity),
                    QString::number(testCases.size()),
                    code,
                    QDate::currentDate().toString(Qt::ISODate)
                };
                if(!isRunButtonClicked)
                    UpdDelAddDataToDB(query, executionInfo);
            }
        });
    }
}
int MainWindow::measureNetworkLatency(const QString& language, const QString& version)
{
    QElapsedTimer timer;
    timer.start();
    QNetworkRequest request(QUrl("https://emkc.org/api/v2/piston/execute"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject pingBody;
    pingBody["language"] = language;
    pingBody["version"] = version;
    pingBody["files"] = QJsonArray();

    QNetworkReply* reply = networkManager->post(request, QJsonDocument(pingBody).toJson());

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    int latency = timer.elapsed();
    reply->deleteLater();

    return latency;
}

void MainWindow::onExecuteButtonClicked()
{
    db.open();

    QString code = ui->plainTextEdit_solution_onProblemPage->toPlainText().trimmed();

    if (code.isEmpty()) {
        loadingWindow.close();
        QMessageBox::warning(this, "Ошибка", "Введите код для выполнения");
        return;
    }

    QString query = "SELECT normilizeTitle, version FROM GC_programmingLanguages WHERE title = ?";
    QString language = ui->comboBox_currentLanguage_onProblemPage->currentText();

    QSqlQuery sqlQuery;
    sqlQuery.prepare(query);
    sqlQuery.addBindValue(language);

    if (!sqlQuery.exec() || !sqlQuery.next()) {
        loadingWindow.close();
        QMessageBox::critical(this, "Ошибка", "Не удалось получить данные о языке");
        return;
    }

    QString normilizeLanguage = sqlQuery.value("normilizeTitle").toString();
    QString version = sqlQuery.value("version").toString();

    QVector<QPair<QString, QString>> testCases = getProblemTestCases(!isRunButtonClicked);


    callPistonApi(testCases, code, version, normilizeLanguage);

    isRunButtonClicked = false;
}


void MainWindow::on_pushButton_sendComment_onProblemPage_clicked()
{
    QVector<QString>data;

    QString comment = ui->plainTextEdit_comment_onProblemPage->toPlainText();
    if(comment.size()==0)
        return;
    data = {problemID, comment, global_userID, QDate::currentDate().toString(Qt::ISODate)};
    QString query = "INSERT INTO GC_problemsDiscuss (problem_id, message, creator_id, creation_date) VALUES (?,?,?,?)";

    UpdDelAddDataToDB(query, data);
    setDiscussTableView(problemID);
    setTableViewVisibility(*ui->tableView_discuss_onProblemPage, *ui->label_emptyData_onProblemPage);
    ui->plainTextEdit_comment_onProblemPage->clear();
}


void MainWindow::on_checkBox_showSubmitResults_onProblemPage_clicked(bool checked)
{
    setWidgetsVisibility(problemPageSolution_widgets, !checked);
    setWidgetsVisibility(problemPageUserCodeExecutionResults_widgets, checked);
}


void MainWindow::on_pushButton_runCode_onProblemPage_clicked()
{
    isRunButtonClicked = true;
    qDebug()<<isRunButtonClicked;
}


void MainWindow::setProblemExecutionResults(const QString& executionStatus, const QPair<QString, QString>& testCase,
                                            int executionTime, QString output, QString passedTestCasesQuantity, QString totalpassedTestCasesQuantity)
{
    ui->label_verdict_onProblemPage->setText(executionStatus);
    ui->label_inputValue_onProblemPage->setText(testCase.first);
    ui->label_outputValue_onProblemPage->setText(testCase.second);
    ui->label_userOutputValue_onProblemPage->setText(output);
    ui->label_userCodeExecutionTimeValue_onProblemPage->setText(QString::number(executionTime));
    ui->label_passedTestCasesQuantity_onProblemPage->setText(passedTestCasesQuantity+"/"+totalpassedTestCasesQuantity);

    setWidgetsVisibility(problemPageSolution_widgets, false);
    setWidgetsVisibility(problemPageUserCodeExecutionResults_widgets, true);
    ui->checkBox_showSubmitResults_onProblemPage->setCheckState(Qt::Checked);


}


void MainWindow::on_pushButton_prevPage_onProblemPage_clicked()
{
    movePage(2);
}








