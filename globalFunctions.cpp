#include <ui_mainwindow.h>
#include <mainwindow.h>


void MainWindow::movePage(int idx)
{
    if(idx > -1)
        ui->stackedWidget->setCurrentIndex(idx);
    else
        ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex()+idx);
}


void MainWindow::callErrorBox(QString boxTitle, QString errorText)
{
    QMessageBox::critical(this, boxTitle, errorText);
}


void MainWindow::callAcceptBox(QString boxTitle, QString text)
{
    QMessageBox::information(this,boxTitle, text);
}


void MainWindow::on_pushButton_prevPage_onInterviewQuestion_clicked()
{
    if(isAdminAcc(global_userName))
        movePage(-1);
    else
        movePage(2);
}


QVector<QString> MainWindow::getSomeValuesFromDB(const QString& queryStr, const QVariantList& params) {
    QSqlQuery query(db);
    query.prepare(queryStr);
    QVector<QString> valuesArr;

    for (const auto& param : params) {
        query.addBindValue(param);
    }

    if(query.exec()){
        while (query.next()) {
            valuesArr.append(query.value(0).toString());
        }
    }

    return valuesArr;
}


QString MainWindow::getValueFromDB(const QString& queryStr, const QVariantList& params) {
    QSqlQuery query(db);
    query.prepare(queryStr);
    for (const auto& param : params) {
        query.addBindValue(param);
    }
    if (query.exec() && query.next()) {
        return query.value(0).toString();
    }
    return QString();
}


QVector<QString> MainWindow::getValuesDifferentColumnsFromDB(const QString &queryStr, const QVariantList& params, const int columnsQuantity)
{
    QVector<QString> result;
    QSqlQuery query(db);

    query.prepare(queryStr);
    for (const auto& param : params) {
        query.addBindValue(param);
    }

    if (!query.exec()) {
        callErrorBox(ERROR_TITLE, DATABASE_ERROR);
    }

    while(query.next()){
        for(int i = 0; i < columnsQuantity; i++)
            result.append(query.value(i).toString());
    }



    return result;
}


bool MainWindow::isAdminAcc(const QString& userName)
{
    if(db.open()){
        QString query, isAdminAcc;
        query = "SELECT isAdmin FROM GC_users WHERE name = :name";
        isAdminAcc = getValueFromDB(query, {userName});

        if (isAdminAcc == "1")
            return true;
        else
            return false;
    }
}


void MainWindow::setLoadingWindow()
{
    loadingWindow.setLabelText("Loading..."); // Устанавливаем текст
    loadingWindow.setWindowModality(Qt::WindowModal); // Делаем диалог модальным
    loadingWindow.setMinimumDuration(0);
    loadingWindow.setCancelButton(nullptr);
    loadingWindow.setRange(0, 100);
    loadingWindow.open();
}


void MainWindow::setTableModel(const QStringList& headers, const QString& query,
                               QTableView& tableView, const QVector<QVariant>& params)
{

    QSqlQueryModel *tableModel = new CustomSqlQueryModel(headers, &tableView);

    QSqlQuery sqlQuery;
    if (!sqlQuery.prepare(query)) {
        delete tableModel;
        return;
    }

    for (const QVariant& param : params) {
        sqlQuery.addBindValue(param);
    }

    if (!sqlQuery.exec()) {
        delete tableModel;
        return;
    }

    tableModel->setQuery(sqlQuery);
    tableView.setModel(tableModel);
    tableView.horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}

void MainWindow::setTagsTableModel(QTableView& tableView)
{
    QStringList tableModelHeaders;
    QString tableModelQuery;

    tableModelQuery = "SELECT title FROM GC_tags";
    tableModelHeaders = {"Теги"};
    setTableModel(tableModelHeaders, tableModelQuery, tableView, {});
}


void MainWindow::addTagsToProblem(QTableView& tableView, QSet<QString>& set, QLabel& label)
{
    QString currentTags_forLabel;

    QString tagTitle = tableView.model()->data(tableView.currentIndex()).toString();
    if(tagTitle.size()!=0){
        if(!set.contains(tagTitle)){
            if(set.size()<7)
                set.insert(tagTitle);
            else
                callErrorBox(ERROR_TITLE, "Вы не можете добавить больше 7 тегов");

        }
        else
            set.remove(tagTitle);
    }

    for(const QString i : set){
        currentTags_forLabel = currentTags_forLabel + " | "+ i;

    }


    label.setText(currentTags_forLabel);
}


void MainWindow::UpdDelAddDataToDB(const QString& query, const QVector<QString>& data)
{

    QSqlQuery addDataQuery(db);
    addDataQuery.prepare(query);
    for (const QString& currentData : data){
        addDataQuery.addBindValue(currentData);
    }

    if (!addDataQuery.exec()){
        callErrorBox(ERROR_TITLE, DATABASE_ERROR);
        return;
    }
}


void MainWindow::setProblemsList(QTableView& tableView)
{
    QString query = "SELECT title, difficulty FROM GC_problems";
    QVector<QString> headers = {"Название", "Сложность"};
    setTableModel(headers, query, tableView, {});
    filterSourceModel = tableView.model();


}


void MainWindow::setFilterLineEdit(const QString& filterText, QAbstractItemModel* sourceTable, const int keyColumnIdx, QTableView& tableView)
{
    QSortFilterProxyModel *filterTable = new QSortFilterProxyModel(this);
    filterTable->setSourceModel(sourceTable);
    filterTable->setFilterKeyColumn(keyColumnIdx);
    tableView.setModel(filterTable);
    filterTable->setFilterFixedString(filterText);

    tableView.horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}


void MainWindow::setTableViewVisibility(QTableView &tableView, QLabel &label)
{
    if(tableView.model()->rowCount() == 0){
        tableView.setVisible(false);
        label.setVisible(true);
    }
    else{
        tableView.setVisible(true);
        label.setVisible(false);
    }
}


void MainWindow::setDiscussTableView(const QString& problemID)
{
    qDebug()<<"ProblemID:"<<problemID;
    QStringList tableModelHeaders = {"Автор", "Комментарий"};
    QString tableModelQuery = "SELECT GC_users.name, GC_problemsDiscuss.message "
                      "FROM GC_users "
                      "INNER JOIN GC_problemsDiscuss "
                      "ON GC_users.id = GC_problemsDiscuss.creator_id "
                      "WHERE GC_problemsDiscuss.problem_id = :problemID";
    setTableModel(tableModelHeaders, tableModelQuery, *ui->tableView_discuss_onProblemPage, {problemID});
    ui->tableView_discuss_onProblemPage->resizeRowsToContents();
    ui->tableView_discuss_onProblemPage->setWordWrap(true);
}


void MainWindow::setInterviewQuestionsTableView(QTableView& tableView, int valuesQuantity)
{
    QVector<QString>data;
    QString questionTitle, getValueQuery, creatorName, questionID;

    QModelIndexList selectedIndexes = tableView.selectionModel()->selectedIndexes();
    QModelIndex selectedIndex = selectedIndexes.at(0);
    questionID = tableView.model()->data(selectedIndex.sibling(selectedIndex.row(), 1)).toString();

    questionTitle = tableView.model()->data(tableView.currentIndex()).toString();

    getValueQuery = "SELECT question, grade, answer, creator_id FROM GC_interviewsQuestions WHERE id = :id";
    data = getValuesDifferentColumnsFromDB(getValueQuery, {questionID}, valuesQuantity);

    getValueQuery = "SELECT name FROM GC_users WHERE GC_users.id = :user_id";
    creatorName = getValueFromDB(getValueQuery, {data[3]});

    data.append(questionTitle);
    data.append(creatorName);
    data.append(questionID);


    ui->label_creatorName_onInterviewQuestion->setText(data[data.size()-2]);
    ui->plainTextEdit_questionDescription_onInterviewQuestion->setPlainText(data[0]);
    ui->plainTextEdit_questionDescription_onInterviewQuestion->setReadOnly(true);
    ui->plainTextEdit_rightAnswer_onInterviewQuestion->setPlainText(data[2]);
    ui->plainTextEdit_rightAnswer_onInterviewQuestion->setReadOnly(true);
    ui->label_grade_onInterviewQuestion->setText(data[1]);
    ui->label_questionTitle_onInterviewQuestion->setText(data[data.size()-3]);


}


void MainWindow::setWidgetsVisibility(const QVector<QWidget*>& widgets, bool isVisible)
{
    for(auto *widget : widgets){
        if (!widget) {
            qCritical() << "Null widget found";
        }
            widget->setVisible(isVisible);
    }

}







