#include "mainwindow.h"
#include "ui_mainwindow.h"

QString questionTitle;

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    QString query;
    if(index == 0)
        query = "SELECT title FROM GC_interviewsQuestions WHERE creator_id = :userID";
    else if(index == 1)
        query = "SELECT title FROM GC_interviewsQuestions WHERE status = 'verified' AND creator_id = :userID";
    else if(index == 2)
        query = "SELECT title FROM GC_interviewsQuestions WHERE status = 'canceled' AND creator_id = :userID";
    else
        query = "SELECT title FROM GC_interviewsQuestions WHERE status = 'unverified' AND creator_id = :userID";


    QVector<QString> headers = {"Вопросы"};
    setTableModel(headers, query, *ui->tableView_allUserQuestions_onAllUserQuestionsList, {global_userID});
    filterSourceModel = ui->tableView_allUserQuestions_onAllUserQuestionsList->model();
    setTableViewVisibility(*ui->tableView_allUserQuestions_onAllUserQuestionsList, *ui->label_emptyData_onAllUserQuestionsList);
}


void MainWindow::on_lineEdit_filter_onAllUserQuestionsList_textChanged(const QString &filterText)
{
    setFilterLineEdit(filterText, filterSourceModel, 0, *ui->tableView_allUserQuestions_onAllUserQuestionsList);
}


void MainWindow::on_tableView_allUserQuestions_onAllUserQuestionsList_clicked()
{
    QString getValueQuery;

    questionTitle = ui->tableView_allUserQuestions_onAllUserQuestionsList->model()->
                    data(ui->tableView_allUserQuestions_onAllUserQuestionsList->currentIndex()).toString();
    qDebug()<<questionTitle;

    QVector<QString>data;

    getValueQuery = "SELECT question, grade, answer, adminFeedback, status FROM GC_interviewsQuestions WHERE title = :title AND "
                    "creator_id = :creator_id";

    data = getValuesDifferentColumnsFromDB(getValueQuery, {questionTitle, global_userID}, 5);


    ui->label_questionTitle_onUserQuestion->setText(questionTitle);
    ui->plainTextEdit_questionDescription_onUserQuestion->setPlainText(data[0]);
    ui->label_grade_onUserQuestion->setText(data[1]);
    ui->plainTextEdit_rightAns_onUserQuestion->setPlainText(data[2]);
    ui->plainTextEdit_adminMessage_onUserQuestion->setPlainText(data[3]);
    ui->label_questionStatus_onUserQuestion->setText(data[4]);


    movePage(14);
}


void MainWindow::on_pushButton_deleteQuestion_onUserQuestion_clicked()
{

    QMessageBox::StandardButton reply;
    QVector<QString>data = {questionTitle, global_userID};
    QString query;
    reply = QMessageBox::question(this,"Подтверждение", "Вы уверены, что хотите перенести данный вопрос в архив?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        setLoadingWindow();
        query = "UPDATE GC_interviewsQuestions set status = 'deleted' WHERE title = :title AND creator_id = :creator_id";
        UpdDelAddDataToDB(query, data);
        callAcceptBox(ACCEPT_TITLE, "Вопрос был успешно удален");
        movePage(-2);
        loadingWindow.close();
    }
}


void MainWindow::on_pushButton_templateUsing_onUserQuestion_clicked()
{
    QString getValueQuery;
    QVector<QString>data;
    getValueQuery = "SELECT question, grade, answer, status FROM GC_interviewsQuestions WHERE title = :title AND "
                    "creator_id = :creator_id";

    data = getValuesDifferentColumnsFromDB(getValueQuery, {questionTitle, global_userID}, 4);
    ui->lineEdit_questionTitle_onCreateQuestion->setText(questionTitle);
    ui->plainTextEdit_question_onCreateQuestion->setPlainText(data[0]);
    ui->comboBox_grades_onCreateQuestion->setCurrentText(data[1]);
    ui->plainTextEdit_rightAnswer_onCreateQuestion->setPlainText(data[2]);

    isTemplateQuestion = true;
    oldQuestionTitle = ui->lineEdit_questionTitle_onCreateQuestion->text();

    movePage(8);

}

