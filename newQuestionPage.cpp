#include <mainwindow.h>
#include <ui_mainwindow.h>


void MainWindow::on_pushButton_addQuestion_onAllQuestions_clicked()
{
    movePage(8);
}


void MainWindow::on_pushButton_createQuestion_onCreateQuestion_clicked()
{

    QString title, question, rightAns, grade, query, currentStatus;
    title = ui->lineEdit_questionTitle_onCreateQuestion->text();
    question = ui->plainTextEdit_question_onCreateQuestion->toPlainText();
    rightAns = ui->plainTextEdit_rightAnswer_onCreateQuestion->toPlainText();
    grade = ui->comboBox_grades_onCreateQuestion->currentText();
    QVector<QString> params;

    query = "SELECT title FROM GC_interviewsQuestions WHERE title = :title AND creator_id = :creator_id";
    QString isAvailableQuestion = getValueFromDB(query, {title, global_userID});


    if(title.size() == 0 || question.size() == 0 || rightAns.size() == 0)
        callErrorBox(ERROR_TITLE, INCORRECT_DATA_ERROR);
    else if(isAvailableQuestion.size() > 0 && isTemplateQuestion == false)
        callErrorBox(ERROR_TITLE, "Вы уже создавали такой вопрос. Проверьте его статус в профиле");
    else{
        currentStatus = "unverified";
        if(isAvailableQuestion.size() == 0 && isTemplateQuestion == false){

            query = "INSERT INTO GC_interviewsQuestions (creator_id, title, question, answer, status, grade, creationDate) VALUES (?,?,?,?,?,?,?)";
            params = {global_userID,title,question,rightAns,currentStatus,grade, QDate::currentDate().toString(Qt::ISODate)};
        }
        else{
            query = "UPDATE GC_interviewsQuestions SET title = :title, question = :question, answer = :answer, status = :status ,grade = :grade, "
                            "adminFeedback = :adminFeedback WHERE title = :where_title AND creator_id = :creator_id";
            params = {title, question, rightAns, currentStatus, grade, "",oldQuestionTitle, global_userID};
            isTemplateQuestion = false;

        }

        UpdDelAddDataToDB(query, params);
        callAcceptBox(ACCEPT_TITLE, "Ваш вопрос перешел на модерацию, позже в уведомлениях появится вердикт");
        movePage(2);

    }
}


