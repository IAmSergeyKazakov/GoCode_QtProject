#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <ui_mainwindow.h>
#include <pistonAPI.h>

#include <customsqlquerymodel.h>
#include <QMainWindow>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlTableModel>
#include <QProgressDialog>
#include <QDate>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include <QVector>
#include <QUrl>
#include <qcustomplot.h>
#include <unordered_map>
#include <algorithm>

Q_DECLARE_METATYPE(QVector<double>)



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:


    void on_pushButton_userRegistration_onGreet_clicked();

    void on_pushButton_CreateAcc_onRegistration_clicked();

    void on_pushButton_userAuth_onGreet_clicked();

    void on_pushButton_createProblem_onNewProblem_clicked();

    void on_pushButton_createProblem_onAdminPanel_clicked();

    void on_tableView_problemTags_onCreateProblem_clicked();

    void on_pushButton_changeProblem_onAdminPanel_clicked();


    void setPrevPage()
    {
        movePage(-1);
    }

    void onExecuteButtonClicked();

    void on_lineEdit_problemsFilter_onProblemsToChangeList_textChanged(const QString &arg1);

    void on_tableView_problemsList_onProblemsToChangeList_clicked();

    void on_pushButton_changeProblem_onChangeProblem_clicked();

    void on_tableView_problemTags_onChangeProblem_clicked();

    void on_pushButton_addTag_onCreateProblem_clicked();

    void on_lineEdit_findProblem_onMain_textChanged(const QString &arg1);

    void on_comboBox_problemDifficulty_onMain_currentIndexChanged();

    void on_comboBox_problemTags_onMain_currentIndexChanged();

    void on_pushButton_interviewQuestions_onMain_clicked();

    void on_pushButton_addQuestion_onAllQuestions_clicked();

    void on_pushButton_createQuestion_onCreateQuestion_clicked();

    void on_pushButton_checkQuestions_onAdminPanel_clicked();

    void on_lineEdit_findQuestion__onNonVerifyQuestions_textChanged(const QString &arg1);

    void on_tableView_nonVerifyQuestions_onNonVerifyQuestions_clicked(const QModelIndex &index);

    void on_pushButton_acceptQuestion_onInterviewQuestion_clicked();

    void on_pushButton_cancellQuestion_onInterviewQuestion_clicked();

    void on_pushButton_prevPage__onNonVerifyQuestions_clicked();

    void on_pushButton_prevPage_onProblemsToChangeList_clicked();

    void on_pushButton_userQuestions__onUserProfile_clicked();

    void on_pushButton_userProfile_onMain_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void on_lineEdit_filter_onAllUserQuestionsList_textChanged(const QString &arg1);

    void on_tableView_allUserQuestions_onAllUserQuestionsList_clicked();

    void on_pushButton_deleteQuestion_onUserQuestion_clicked();

    void on_pushButton_templateUsing_onUserQuestion_clicked();

    void on_tableView_problemsList_onMain_clicked(const QModelIndex &index);

    void on_pushButton_sendComment_onProblemPage_clicked();

    void on_pushButton_prevPage_onProblemPage_clicked();

    void on_tableView_allQuestions_onAllQuestions_clicked();

    void on_pushButton_prevPage_onInterviewQuestion_clicked();

    void on_pushButton_runCode_onProblemPage_clicked();

    void on_checkBox_showSubmitResults_onProblemPage_clicked(bool checked);

    void on_tableView_allUserSubmissions_onUserProfile_clicked(const QModelIndex &index);

    void on_pushButton_prevPage_onUserProfile_clicked();

private:
    Ui::MainWindow *ui;

    bool isValueAvailable(const QString& val);
    bool isAdminAcc(const QString& userName);
    void setDatabase();
    void setLoadingWindow();
    void callErrorBox(const QString boxTitle, const QString errorText);
    void callAcceptBox(const QString boxTitle, const QString text);
    void movePage(int idxPage);
    void setConnections();
    void setTableModel(const QStringList& headers, const QString& query, QTableView& tableView, const QVector<QVariant>& params);
    void setTagsTableModel(QTableView& tableView);
    void addTagsToProblem(QTableView& tableView, QSet<QString>& set, QLabel& label);
    void UpdDelAddDataToDB(const QString& query, const QVector<QString>& data);
    void setProblemsList(QTableView& tableView);
    void setFilterLineEdit(const QString& filterText, QAbstractItemModel* sourceTable, const int keyColumnIdx, QTableView& tableView);
    void setQueryBySeveralComboBox();
    void setQueryByComboBox(const QComboBox& comboBox);
    void setComboBoxesBehaviour();
    void setTableViewVisibility(QTableView& tableView, QLabel& label);
    void setDiscussTableView(const QString& problemID);
    void setWidgetsVisibility(const QVector<QWidget*>& widgets, bool isVisible);
    void setInterviewQuestionsTableView(QTableView& tableView, int valuesQuantity);
    void callPistonApi(QVector<QPair<QString, QString>>& testCases, QString& code, QString& version, QString& normilizeLanguage);
    void setProblemExecutionResults(const QString& executionStatus, const QPair<QString, QString>& testCase,
                                    int executionTime, QString output, QString passedTestCasesQuantity, QString totalTestCasesQuantity);
    void setSubmissionsChart();
    void createSentChart(const QMap<QDate, int>& dateCountMap);

    QString getValueFromDB(const QString& queryStr, const QVariantList& params);
    QString hashPassword(const QString &password);
    QString normalizeLanguageName(const QString &lang) const;

    QVector<QString> getSomeValuesFromDB(const QString& queryStr, const QVariantList& params);
    QVector<QString> getValuesDifferentColumnsFromDB(const QString &queryStr, const QVariantList& params, const int columnsQuantity);

    QVector<QPair<QString, QString>> getProblemTestCases(bool isSubmit);

    int measureNetworkLatency(const QString& language, const QString& version);





    QString global_userName, global_userID, getValueQuery, oldQuestionTitle, problemID;

    const QString ERROR_TITLE = "Ошибка";
    const QString INCORRECT_DATA_ERROR = "Некорректные данные";
    const QString DATABASE_ERROR = "Не удалось подключиться к базе данных";
    const QString UNKNOWN_ERROR = "Неизвестная ошибка";
    const QString ACCEPT_TITLE = "Успешно";
    const QString ACCEPT_TEXT = "Все готово!";
    const QString TL_problemVerdict = "Превышенно ограничение по времени";
    const QString RE_problemVerdict = "Ошибка выполнения (вероятно, в вашем коде ошибка)";
    const QString WA_problemVerdict = "Неверный ответ";
    const QString OK_problemVerdict = "Полное решение";
    const QString CREATE_PROBLEM_BUTTON_HINT = "Чтобы добавить задачу вам нужно прикрепить файл формата .xlsx"
                                               "\nИмя первой колонки должно быть 'Input'"
                                               "\nИмя второй колонки должно быть 'Output'"
                                               "\nДалее - заполнять строки строго Input - Output";


    QVector<QWidget*> problemPageUserCodeExecutionResults_widgets;
    QVector<QWidget*> problemPageSolution_widgets;


    QSqlDatabase db;
    QProgressDialog loadingWindow;
    QAbstractItemModel* filterSourceModel;
    QAbstractItemModel* filterSourceModelOnProblemsListToChange;

    bool isTemplateQuestion = false;

    QNetworkAccessManager *networkManager;




};
#endif // MAINWINDOW_H
