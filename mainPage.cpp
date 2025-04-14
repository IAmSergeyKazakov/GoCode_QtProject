#include <mainwindow.h>
#include <ui_mainwindow.h>
#include <QComboBox>

QAbstractItemModel* mainProblemsList;
QString problemID;


void MainWindow::setQueryBySeveralComboBox()
{
    QString query;

        QString tagId = getValueFromDB("SELECT id FROM GC_tags WHERE title = :title", {ui->comboBox_problemTags_onMain->currentText()});

        query = "SELECT GC_problems.title, GC_problems.difficulty "
                "FROM GC_problems "
                "INNER JOIN GC_problemsTags ON GC_problems.id = GC_problemsTags.problem_id "
                "WHERE GC_problemsTags.tag_id = :tagId AND GC_problems.difficulty = :difficulty ";
        QVector<QString> headers = {"Название", "Сложность"};
        setTableModel(headers, query, *ui->tableView_problemsList_onMain, {tagId, ui->comboBox_problemDifficulty_onMain->currentText()});

        filterSourceModel = ui->tableView_problemsList_onMain->model();

}


void MainWindow::setQueryByComboBox(const QComboBox& comboBox)
{

    QString query;

    QString tagId = getValueFromDB("SELECT id FROM GC_tags WHERE title = :title", {comboBox.currentText()});
    QVector<QVariant> filledData;

    if(&comboBox == ui->comboBox_problemDifficulty_onMain){
        query = "SELECT title, difficulty FROM GC_problems WHERE difficulty = :currentText";
        filledData = {comboBox.currentText()};
    }
    else{
        query = "SELECT GC_problems.title, GC_problems.difficulty "
                        "FROM GC_problems "
                        "INNER JOIN GC_problemsTags ON GC_problems.id = GC_problemsTags.problem_id "
                        "WHERE GC_problemsTags.tag_id = :tagId";
        filledData = {tagId};
    }

    QVector<QString> headers = {"Название", "Сложность"};
    setTableModel(headers, query, *ui->tableView_problemsList_onMain, {filledData});
    filterSourceModel = ui->tableView_problemsList_onMain->model();
}


void MainWindow::setComboBoxesBehaviour()
{
    if(ui->comboBox_problemTags_onMain->currentText() == "Все" && ui->comboBox_problemDifficulty_onMain->currentText() == "Все")
        setProblemsList(*ui->tableView_problemsList_onMain);

    else if(ui->comboBox_problemTags_onMain->currentText() != "Все" && ui->comboBox_problemDifficulty_onMain->currentText() == "Все"){
        setQueryByComboBox(*ui->comboBox_problemTags_onMain);
    }
    else if(ui->comboBox_problemTags_onMain->currentText() == "Все" && ui->comboBox_problemDifficulty_onMain->currentText() != "Все")
        setQueryByComboBox(*ui->comboBox_problemDifficulty_onMain);
    else
        setQueryBySeveralComboBox();

    setTableViewVisibility(*ui->tableView_problemsList_onMain, *ui->label_emptyData_onMain);

}


void MainWindow::on_lineEdit_findProblem_onMain_textChanged(const QString &filterText)
{
    setFilterLineEdit(filterText, filterSourceModel, 0, *ui->tableView_problemsList_onMain);
}


void MainWindow::on_comboBox_problemDifficulty_onMain_currentIndexChanged()
{
    setComboBoxesBehaviour();
}


void MainWindow::on_comboBox_problemTags_onMain_currentIndexChanged()
{
    setComboBoxesBehaviour();
}


void MainWindow::on_pushButton_interviewQuestions_onMain_clicked()
{
    QString tableModelQuery = "SELECT title, id FROM GC_interviewsQuestions WHERE status = 'verified'";
    QStringList tableModelHeaders = {"Вопросы", "id"};
    setTableModel(tableModelHeaders, tableModelQuery, *ui->tableView_allQuestions_onAllQuestions, {});

    ui->tableView_allQuestions_onAllQuestions->setColumnHidden(1, true);
    setTableViewVisibility(*ui->tableView_allQuestions_onAllQuestions, *ui->label_emptyData_onAllQuestions);
    movePage(7);
}


void MainWindow::on_pushButton_userProfile_onMain_clicked()
{
    setSubmissionsChart();


    QString query = "SELECT COUNT(*) as userSubmissions FROM GC_submissions WHERE user_id = :userID";
    int totalUserSubmissions = getValueFromDB(query, {global_userID}).toInt();

    query = "SELECT COUNT(*) as userSubmissions FROM GC_submissions WHERE user_id = :userID AND verdict = 'OK'";
    int acceptedUserSubmissions = getValueFromDB(query, {global_userID}).toInt();

    qDebug()<<totalUserSubmissions;
    qDebug()<<acceptedUserSubmissions;

    int userAcceptance = 0;

    if (acceptedUserSubmissions > 0 && totalUserSubmissions > 0) {
        userAcceptance = (100 * acceptedUserSubmissions) / totalUserSubmissions;
    }

    ui->label_userAcceptance_onUserProfile->setText(QString::number(userAcceptance) + "%");
    ui->label_name_onUserProfile->setText(global_userName);

    QString tableModelQuery = "SELECT GC_submissions.id,GC_submissions.code, GC_problems.title, GC_submissions.verdict, "
                              "GC_submissions.passedTestCasesQuantity, GC_submissions.totalTestCasesQuantity "
                              "FROM GC_problems "
                              "INNER JOIN GC_submissions "
                              "ON GC_submissions.problem_id = GC_problems.id "
                              "WHERE GC_submissions.user_id = :global_userID "
                              "ORDER BY GC_submissions.creation_date DESC";
    QStringList tableModelHeaders = {"id", "code", "Задача", "Результат", "Пройденные тесты", "Общее кол-во тестов"};

    setTableModel(tableModelHeaders, tableModelQuery, *ui->tableView_allUserSubmissions_onUserProfile, {global_userID});
    ui->tableView_allUserSubmissions_onUserProfile->setColumnHidden(0, true);
    ui->tableView_allUserSubmissions_onUserProfile->setColumnHidden(1, true);

    QVector<QWidget*> userCodeOnProfile = {ui->label_userCode_onUserProfile, ui->plainText_userCode_onUserProfile};
    setWidgetsVisibility(userCodeOnProfile, false);

    movePage(12);
}


void MainWindow::on_tableView_problemsList_onMain_clicked(const QModelIndex &index)
{
    db.open();
    setLoadingWindow();
    QString getValueQuery, problemTitle, tags;
    QVector<QString>data, tagsArr;

    QAbstractItemModel* model = ui->tableView_problemsList_onMain->model();
    problemTitle = model->data(model->index(index.row(), 0)).toString();

    getValueQuery = "SELECT id from GC_problems WHERE title = :title";
    problemID = getValueFromDB(getValueQuery, {problemTitle});

    getValueQuery = "SELECT description, difficulty, time_limit FROM GC_problems WHERE id = :problem_id";
    data = getValuesDifferentColumnsFromDB(getValueQuery, {problemID}, 3);

    getValueQuery = "SELECT GC_tags.title FROM GC_tags INNER JOIN GC_problemsTags "
                    "ON GC_problemsTags.tag_id = GC_tags.id WHERE GC_problemsTags.problem_id = :problem_id";
    tagsArr = getSomeValuesFromDB(getValueQuery, {problemID});

    QStringList tableModelHeaders = {"Вход", "Выход"};
    QString tableModelQuery = "SELECT input, output FROM GC_testCases WHERE problem_id = :problemID";
    setTableModel(tableModelHeaders, tableModelQuery, *ui->tableView_testCases_onProblemPage, {problemID});

    getValueQuery = "SELECT title FROM GC_programmingLanguages";
    QStringList programmingLanguages = (getSomeValuesFromDB(getValueQuery, {}).toList());
    ui->comboBox_currentLanguage_onProblemPage->addItems(programmingLanguages);


    setDiscussTableView(problemID);

    for (QString currentTag : tagsArr){
        tags+=(currentTag+" | ");
    }

    ui->label_problemTitle_onProblemPage->setText(problemTitle);
    ui->plainTextEdit_problemDescription_onProblemPage->setPlainText(data[0]);
    ui->plainTextEdit_problemDescription_onProblemPage->setReadOnly(true);
    ui->label_diffculty_onProblemPage->setText(data[1]);
    ui->label_timeLimit_onProblemPage->setText(data[2]+" мс");
    ui->label_problemTags_onProblemPage->setText(tags);

    setTableViewVisibility(*ui->tableView_discuss_onProblemPage, *ui->label_emptyData_onProblemPage);

    loadingWindow.close();



    setWidgetsVisibility(problemPageSolution_widgets, true);
    setWidgetsVisibility(problemPageUserCodeExecutionResults_widgets, false);


    movePage(11);

}




