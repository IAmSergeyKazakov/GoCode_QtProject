#include <ui_mainwindow.h>
#include <mainwindow.h>


void MainWindow::on_pushButton_userQuestions__onUserProfile_clicked()
{
    QString tableModelQuery = "SELECT title FROM GC_interviewsQuestions WHERE creator_id = userID AND status != 'deleted'";
    QStringList tableModelHeaders = {"Вопросы"};

    setTableModel(tableModelHeaders, tableModelQuery, *ui->tableView_allUserQuestions_onAllUserQuestionsList, {global_userID});
    filterSourceModel = ui->tableView_allUserQuestions_onAllUserQuestionsList->model();
    setTableViewVisibility(*ui->tableView_allUserQuestions_onAllUserQuestionsList, *ui->label_emptyData_onAllUserQuestionsList);
    movePage(13);
}


#include <QPropertyAnimation>
#include <QEasingCurve>

void MainWindow::createSentChart(const QMap<QDate, int>& dateCountMap)
{
    QLayout* layout = ui->widget_submissionsChart_onUserProfile->layout();
    if (!layout) {
        layout = new QVBoxLayout(ui->widget_submissionsChart_onUserProfile);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
    } else {
        QLayoutItem* item;
        while ((item = layout->takeAt(0))) {
            delete item->widget();
            delete item;
        }
    }


    QWidget *mainContainer = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(mainContainer);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);


    QWidget *graphWidget = new QWidget();
    QVBoxLayout *graphLayout = new QVBoxLayout(graphWidget);
    graphLayout->setContentsMargins(0, 0, 0, 15);


    QCustomPlot *plot = new QCustomPlot();
    plot->setInteraction(QCP::iRangeZoom, false);
    plot->setInteraction(QCP::iRangeDrag, false);

    QVector<double> x, y;
    QVector<QString> labels;
    QVector<QDate> allDates;
    int currentDateIndex = -1;
    QDate today = QDate::currentDate();

    int pastDays = 7;
    int futureDays = 14;

    if (dateCountMap.isEmpty()) {
        for (int d = -pastDays; d <= futureDays; ++d) {
            QDate date = today.addDays(d);
            allDates << date;
            if (date == today) currentDateIndex = allDates.size()-1;
        }
    } else {
        QDate firstDate = dateCountMap.firstKey();
        QDate lastDate = dateCountMap.lastKey();

        for (int d = -pastDays; d < 0; ++d) {
            allDates << firstDate.addDays(d);
        }

        for (auto it = dateCountMap.constBegin(); it != dateCountMap.constEnd(); ++it) {
            allDates << it.key();
            if (it.key() == today) currentDateIndex = allDates.size()-1;
        }

        for (int d = 1; d <= futureDays; ++d) {
            QDate date = lastDate.addDays(d);
            allDates << date;
            if (date == today) currentDateIndex = allDates.size()-1;
        }
    }

    if (currentDateIndex == -1) {
        allDates << today;
        std::sort(allDates.begin(), allDates.end());
        currentDateIndex = allDates.indexOf(today);
    }

    for (int i = 0; i < allDates.size(); ++i) {
        x << i;
        y << dateCountMap.value(allDates[i], 0);
        labels << allDates[i].toString("dd.MM");
    }

    plot->addGraph();
    plot->graph(0)->setData(x, y);
    plot->graph(0)->setPen(QPen(Qt::blue, 2));
    plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::red, 6));

    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    for (int i = 0; i < labels.size(); ++i) {
        textTicker->addTick(x[i], labels[i]);
    }
    plot->xAxis->setTicker(textTicker);
    plot->xAxis->setTickLabelRotation(60);


    double yMax = *std::max_element(y.constBegin(), y.constEnd());
    plot->yAxis->setRange(0, yMax + (yMax > 0 ? 1 : 0.5));
    plot->xAxis->setRange(-0.5, x.size()-0.5);


    int pointWidth = 50;
    int plotHeight = 300;
    int plotWidth = qMax(x.size() * pointWidth, 800);
    plot->setMinimumSize(plotWidth, plotHeight);

    graphLayout->addWidget(plot);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(false);
    scrollArea->setWidget(graphWidget);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto updateYAxisPosition = [=](int scrollValue) {

        double axisOffset = -scrollValue;
        plot->yAxis->setOffset(axisOffset);

    };

    connect(scrollArea->horizontalScrollBar(), &QScrollBar::valueChanged, [=](int value) {
        updateYAxisPosition(value);
        plot->replot();
    });

    mainLayout->addWidget(scrollArea);
    layout->addWidget(mainContainer);


    plot->replot();

    QTimer::singleShot(100, [=]() {
        plot->resize(plotWidth, plotHeight);

        int viewportWidth = scrollArea->viewport()->width();
        int targetPos = currentDateIndex * pointWidth - viewportWidth/2 + pointWidth/2;
        targetPos = qBound(0, targetPos, plotWidth - viewportWidth);

        scrollArea->horizontalScrollBar()->setRange(0, plotWidth - viewportWidth);


        QPropertyAnimation *animation = new QPropertyAnimation(scrollArea->horizontalScrollBar(), "value");
        animation->setDuration(500);
        animation->setStartValue(scrollArea->horizontalScrollBar()->value());
        animation->setEndValue(targetPos);
        animation->setEasingCurve(QEasingCurve::OutQuad);

        connect(animation, &QPropertyAnimation::valueChanged, [=](const QVariant &value) {
            updateYAxisPosition(value.toInt());
        });

        animation->start(QAbstractAnimation::DeleteWhenStopped);
    });
}


void MainWindow::setSubmissionsChart()
{
    QMap<QDate, int> stats;

    QSqlQuery query;
    query.prepare("SELECT DATE(creation_date) as date, "
                  "COUNT(*) as count "
                  "FROM GC_submissions "
                  "WHERE user_id = :user_id "
                  "GROUP BY DATE(creation_date) "
                  "ORDER BY date");
    query.bindValue(":user_id", global_userID);

    if (query.exec()) {
        while (query.next()) {
            QDate date = query.value("date").toDate();
            int count = query.value("count").toInt();
            stats[date] = count;
        }
    }

    createSentChart(stats);
}


void MainWindow::on_tableView_allUserSubmissions_onUserProfile_clicked(const QModelIndex &index)
{
    QAbstractItemModel* model = ui->tableView_allUserSubmissions_onUserProfile->model();
    QString problemID = model->data(model->index(index.row(), 0)).toString();
    QString userCode = model->data(model->index(index.row(), 1)).toString();

    QVector<QWidget*> userCodeOnProfile = {ui->label_userCode_onUserProfile, ui->plainText_userCode_onUserProfile};
    setWidgetsVisibility(userCodeOnProfile, true);
    ui->plainText_userCode_onUserProfile->setReadOnly(true);
    ui->plainText_userCode_onUserProfile->setPlainText(userCode);
}


void MainWindow::on_pushButton_prevPage_onUserProfile_clicked()
{
    movePage(2);
}
