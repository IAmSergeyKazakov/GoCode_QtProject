#include "mainwindow.h"
#include "ui_mainwindow.h"


void MainWindow::on_tableView_allQuestions_onAllQuestions_clicked()
{
    QVector<QString>data;
    const int valuesQuantity = 4;

    setInterviewQuestionsTableView(*ui->tableView_allQuestions_onAllQuestions, valuesQuantity);

    QVector<QWidget*> interviewQuestionAdminWidgets = {
        ui->pushButton_acceptQuestion_onInterviewQuestion,
        ui->pushButton_cancellQuestion_onInterviewQuestion,
        ui->plainTextEdit_messageToUser_onInterviewQuestion,
        ui->label_userMessage_onInterviewQuestion

    };

    setWidgetsVisibility(interviewQuestionAdminWidgets, false);
    movePage(10);
}
