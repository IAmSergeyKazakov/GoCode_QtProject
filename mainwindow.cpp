#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCryptographicHash>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , networkManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    qRegisterMetaType<QVector<double>>("QVector<double>");
    problemPageUserCodeExecutionResults_widgets = {

        ui->label_input_onProblemPage,
        ui->label_inputValue_onProblemPage,
        ui->label_output_onProblemPage,
        ui->label_outputValue_onProblemPage,
        ui->label_userOutput_onProblemPage,
        ui->label_userOutputValue_onProblemPage,
        ui->label_userCodeExecutionTime_onProblemPage,
        ui->label_userCodeExecutionTimeValue_onProblemPage,
        ui->label_verdict_onProblemPage

    };

    problemPageSolution_widgets = {
        ui->pushButton_runCode_onProblemPage,
        ui->pushButton_submitCode_onProblemPage,
        ui->label_output_onProblemPage,
        ui->plainTextEdit_solution_onProblemPage,
    };

    ui->stackedWidget->setCurrentIndex(0);
    setDatabase();
    setConnections();


}

MainWindow::~MainWindow()
{
    delete ui;
    delete networkManager;

}


QString MainWindow::hashPassword(const QString &password) {
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return QString(hash.toHex());
}


void MainWindow::setConnections()
{
    connect(ui->pushButton_prevPage_onRegistration, &QPushButton::clicked, this, &MainWindow::setPrevPage);
    connect(ui->pushButton_prevPage_onCreateProblem, &QPushButton::clicked, this, &MainWindow::setPrevPage);
    connect(ui->pushButton_prevPage_onChangeProblem, &QPushButton::clicked, this, &MainWindow::setPrevPage);
    connect(ui->pushButton_prevPage_onAllUserQuestionsList, &QPushButton::clicked, this, &MainWindow::setPrevPage);
    connect(ui->pushButton_submitCode_onProblemPage, &QPushButton::clicked, this, &MainWindow::onExecuteButtonClicked);
    connect(ui->pushButton_runCode_onProblemPage, &QPushButton::clicked, this, &MainWindow::onExecuteButtonClicked);
}

