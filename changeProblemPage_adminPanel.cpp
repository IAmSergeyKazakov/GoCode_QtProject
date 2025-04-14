#include <mainwindow.h>
#include <ui_mainwindow.h>

QSet<QString>newTags;
static QString problemID;

void MainWindow::on_tableView_problemsList_onProblemsToChangeList_clicked()
{
    QVector<QString>data;
    newTags.clear();
    QString problemTitle, getValueQuery, currentTagsString;

    if (db.open()){
        setLoadingWindow();

        problemTitle = ui->tableView_problemsList_onProblemsToChangeList->model()->
                       data(ui->tableView_problemsList_onProblemsToChangeList->currentIndex()).toString();

        getValueQuery = "SELECT description, difficulty, time_limit, id FROM GC_problems WHERE title = :title";

        data = getValuesDifferentColumnsFromDB(getValueQuery, {problemTitle}, 4);
        problemID = data[3];

        ui->label_problemTitle_onChangeProblem->setText(problemTitle);

        ui->plainTextEdit_problemDescription_onChangeProblem->clear();
        ui->plainTextEdit_problemDescription_onChangeProblem->insertPlainText(data[0]);

        ui->comboBox_problemDifficulty_onChangeProblem->setCurrentText(data[1]);
        ui->spinBox_problemTimeLimit_onChangeProblem->setValue(data[2].toInt());

        setTagsTableModel(*ui->tableView_problemTags_onChangeProblem);

        getValueQuery = "SELECT GC_tags.title FROM GC_tags INNER JOIN GC_problemsTags ON "
                        "GC_problemsTags.tag_id = GC_tags.id WHERE GC_problemsTags.problem_id = :problem_id";
        QVector<QString> problemTags = getSomeValuesFromDB(getValueQuery, {data[3]});

        for(auto& currentTag : problemTags){
            currentTagsString+=currentTag + " | ";
            newTags.insert(currentTag);
        }
        ui->label_addedTags_onChangeProblem->setText(currentTagsString);
        ui->lineEdit_newTitle_onChangeProblem->setText(problemTitle);

        movePage(6);

        loadingWindow.close();

    }
    else{
        callErrorBox(ERROR_TITLE, DATABASE_ERROR);
    }
}


void MainWindow::on_lineEdit_problemsFilter_onProblemsToChangeList_textChanged(const QString &filterText)
{
    setFilterLineEdit(filterText, filterSourceModel, 0, *ui->tableView_problemsList_onProblemsToChangeList);
}




void MainWindow::on_pushButton_changeProblem_onChangeProblem_clicked()
{

    QString newTitle, newDescription, newTimeLimit, newDifficulty;
    newTitle = ui->lineEdit_newTitle_onChangeProblem->text();
    newDescription = ui->plainTextEdit_problemDescription_onChangeProblem->toPlainText();
    newTimeLimit = QString::number(ui->spinBox_problemTimeLimit_onChangeProblem->value());
    newDifficulty = ui->comboBox_problemDifficulty_onChangeProblem->currentText();

    if(newDescription.size() == 0 || newTitle.size() == 0 || newTags.size() == 0)
        callErrorBox(ERROR_TITLE, INCORRECT_DATA_ERROR);
    else
    {
        setLoadingWindow();
        QString query = "UPDATE GC_problems SET title = :newTitle, time_limit = :newTimeLimit, description = :newDescription"
                        ", difficulty = :newDifficulty WHERE id = :id";
        QVector<QString> data = {newTitle, newTimeLimit, newDescription, newDifficulty, problemID};
        UpdDelAddDataToDB(query, data);

        query = "DELETE FROM GC_problemsTags WHERE problem_id = :problem_id";
        data = {problemID};
        UpdDelAddDataToDB(query, data);

        for(QString currentTag : newTags){

            query = "SELECT id FROM GC_tags WHERE title = :title";
            QString tagId = getValueFromDB(query, {currentTag});

            query = "INSERT INTO GC_problemsTags (problem_id, tag_id) VALUES (?,?)";
            data = {problemID, tagId};
            UpdDelAddDataToDB(query,data);

        }
        loadingWindow.close();

        callAcceptBox(ACCEPT_TITLE, ACCEPT_TEXT);
        movePage(-3);
    }
}


void MainWindow::on_pushButton_changeProblem_onAdminPanel_clicked()
{
    QStringList tableModelHeaders;
    QString tableModelQuery;

    setProblemsList(*ui->tableView_problemsList_onProblemsToChangeList);
    movePage(5);
}


void MainWindow::on_tableView_problemTags_onChangeProblem_clicked()
{
    addTagsToProblem(*ui->tableView_problemTags_onChangeProblem, newTags, *ui->label_addedTags_onChangeProblem);
}


void MainWindow::on_pushButton_prevPage_onProblemsToChangeList_clicked()
{
    movePage(3);
}
