#include <mainwindow.h>
#include <ui_mainwindow.h>

QString questionID;


void MainWindow::on_lineEdit_findQuestion__onNonVerifyQuestions_textChanged(const QString &filterText)
{
    setFilterLineEdit(filterText, filterSourceModel, 0, *ui->tableView_nonVerifyQuestions_onNonVerifyQuestions);
}


void MainWindow::on_pushButton_checkQuestions_onAdminPanel_clicked()
{

    QString tableModelQuery = "SELECT title, id FROM GC_interviewsQuestions WHERE status = 'unverified'";
    QStringList tableModelHeaders = {"Вопросы", "ID"};

    setTableModel(tableModelHeaders, tableModelQuery, *ui->tableView_nonVerifyQuestions_onNonVerifyQuestions, {});
    filterSourceModel = ui->tableView_nonVerifyQuestions_onNonVerifyQuestions->model();
    setTableViewVisibility(*ui->tableView_nonVerifyQuestions_onNonVerifyQuestions, *ui->label_emptyData_onNonVerifyQuestions);
    movePage(9);
}


void MainWindow::on_tableView_nonVerifyQuestions_onNonVerifyQuestions_clicked(const QModelIndex &index)
{
    const int valuesQuantity = 4;

    setInterviewQuestionsTableView(*ui->tableView_nonVerifyQuestions_onNonVerifyQuestions, valuesQuantity);

    QVector<QWidget*> interviewQuestionAdminWidgets = {
        ui->pushButton_acceptQuestion_onInterviewQuestion,
        ui->pushButton_cancellQuestion_onInterviewQuestion,
        ui->plainTextEdit_messageToUser_onInterviewQuestion,
        ui->label_userMessage_onInterviewQuestion

    };
    QAbstractItemModel* model = ui->tableView_nonVerifyQuestions_onNonVerifyQuestions->model();
    questionID = model->data(model->index(index.row(), 1)).toString();

    setWidgetsVisibility(interviewQuestionAdminWidgets, true);

    movePage(10);
}


void MainWindow::on_pushButton_acceptQuestion_onInterviewQuestion_clicked()
{
    QString query, adminMessage;
    query = "UPDATE GC_interviewsQuestions SET status = 'verified', adminFeedback = :adminFeedback WHERE id = :questionID";

    adminMessage = ui->plainTextEdit_messageToUser_onInterviewQuestion->toPlainText();
    QVector<QString>data = {adminMessage,questionID};
    qDebug()<<query<<" "<<adminMessage<<questionID;
    UpdDelAddDataToDB(query, data);

    callAcceptBox(ACCEPT_TITLE, "Вопрос успешно одобрен");
}


void MainWindow::on_pushButton_cancellQuestion_onInterviewQuestion_clicked()
{
    QString query, adminMessage;
    query = "UPDATE GC_interviewsQuestions SET status = 'canceled', adminFeedback = :adminFeedback WHERE id = :questionID";
    adminMessage = ui->plainTextEdit_messageToUser_onInterviewQuestion->toPlainText();
    QVector<QString>data = {adminMessage,questionID};
    UpdDelAddDataToDB(query, data);
    callAcceptBox(ACCEPT_TITLE, "Вопрос успешно отклонен");
}


void MainWindow::on_pushButton_prevPage__onNonVerifyQuestions_clicked()
{
    movePage(3);
}

