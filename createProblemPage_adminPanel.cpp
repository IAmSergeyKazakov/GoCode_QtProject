#include <ui_mainwindow.h>
#include <mainwindow.h>
#include <QSet>
#include <QFileDialog>
#include "xlsxdocument.h"


QSet<QString>currentTags;

void MainWindow::on_pushButton_createProblem_onNewProblem_clicked()
{
    QString problemTitle = ui->lineEdit_problemTitle_onCreateProblem->text();
    QString problemDescription = ui->plainTextEdit_problemDescription_onCreateProblem->toPlainText();
    QString problemDifficulty = ui->comboBox_problemDifficulty__onCreateProblem->currentText();
    QString query;
    QVector<QString> data;
    bool isTestCasesCorrect = false;

    int problemTimeLimit = ui->spinBox_problemTimeLimit__onCreateProblem->value();

    getValueQuery = "SELECT id FROM GC_problems WHERE title = :title";
    QString currentProblemId = getValueFromDB(getValueQuery, {problemTitle});
    if (db.open()){

        if (problemTitle.size()>0 && problemDescription.size()>0 && problemTimeLimit >= 1000 && currentTags.size()>0 && currentProblemId.size() == 0)
        {
            setLoadingWindow();

            query = "INSERT INTO GC_problems (title, difficulty,description, time_limit,creation_date) VALUES (?,?,?,?,?)";
            data = {problemTitle, problemDifficulty, problemDescription,
                                     QString::number(problemTimeLimit),
                                     QDate::currentDate().toString(Qt::ISODate)};
            UpdDelAddDataToDB(query, data);

            getValueQuery = "SELECT id FROM GC_problems WHERE title = :title";
            QString addedProblemId = getValueFromDB(getValueQuery, {problemTitle});

            for (const QString &currentTag : currentTags){

                getValueQuery = "SELECT id FROM GC_tags WHERE title = :title";
                QString tagId = getValueFromDB(getValueQuery, {currentTag});
                query = "INSERT INTO GC_problemsTags (problem_id, tag_id) VALUES (?,?)";
                data = {addedProblemId, tagId};
                UpdDelAddDataToDB(query,data);
            }

            loadingWindow.close();


            QString filePath = QFileDialog::getOpenFileName(this, "Выберите файл Excel", "", "Excel Files (*.xlsx)");
            QXlsx::Document testCasesXslxFile(filePath);

            int currentXslxRow = 2;

            if(testCasesXslxFile.read(1, 1).toString() == "Input" || testCasesXslxFile.read(1, 2).toString() == "Output"
                                                                          && !filePath.isEmpty() && testCasesXslxFile.load())
            {
                while (true)
                {
                    QString inputVal = testCasesXslxFile.read(currentXslxRow, 1).toString();
                    QString outputVal = testCasesXslxFile.read(currentXslxRow, 2).toString();
                    if(inputVal.isEmpty() && outputVal.isEmpty())
                    {
                        callAcceptBox(ACCEPT_TITLE, QString("Задача "+ problemTitle+ " добавлена"));
                        isTestCasesCorrect = true;
                        break;
                    }

                    query = "INSERT INTO GC_testCases (problem_id, input, output) VALUES (?, ?, ?)";
                    data = {addedProblemId, inputVal, outputVal};
                    UpdDelAddDataToDB(query, data);

                    currentXslxRow++;
                }
            }
            else if (testCasesXslxFile.read(1, 1).toString() != "Input" || testCasesXslxFile.read(1, 2).toString() != "Output")
                callErrorBox(ERROR_TITLE, "Первая строчка первой колонки в файле Excel должна быть 'Input', а вторая колонка первой строчки - 'Output'");

            if(!isTestCasesCorrect){
                query = "DELETE FROM GC_problems WHERE id = :problem_id";
                data = {addedProblemId};
                UpdDelAddDataToDB(query, data);
                callErrorBox(ERROR_TITLE, "Тестовые случаи не были загружены. Задача не создана\n"
                                          "Проверьте корректность файла .xlsx");
            }

        }
        else if(currentProblemId.size()!=0)
                callErrorBox(ERROR_TITLE, "Задача с таким названием уже существует");
        else if(currentTags.size()<1)
                callErrorBox(ERROR_TITLE, "Вы должны добавить хотя бы 1 тег к задаче");
        else if(problemTimeLimit < 1000)
                callErrorBox(ERROR_TITLE, "Time Limit должен быть не менее 1000мс");
        else if(problemTimeLimit > 10000)
                callErrorBox(ERROR_TITLE, "Time Limit должен быть не более 10000мс");
        else{
            callErrorBox(ERROR_TITLE, INCORRECT_DATA_ERROR);
        }
    }
    else{
        callErrorBox(ERROR_TITLE, DATABASE_ERROR);
    }


    movePage(4);
}


void MainWindow::on_tableView_problemTags_onCreateProblem_clicked()
{
    addTagsToProblem(*ui->tableView_problemTags_onCreateProblem, currentTags, *ui->label_addedTags_onCreateProblem);
}


void MainWindow::on_pushButton_addTag_onCreateProblem_clicked()
{
    QString addedTagText = ui->lineEdit_addTag_onCreateProblem->text();
    QString query;
    if(addedTagText.size() != 0 && addedTagText.size() <= 20)
    {
        setLoadingWindow();
        query = "SELECT id FROM GC_tags WHERE title = :title";
        QString tagId = getValueFromDB(query, {addedTagText});

        if(tagId.size() > 0){
            callErrorBox(ERROR_TITLE, "Такой тег уже существует");
            ui->lineEdit_addTag_onCreateProblem->clear();
        }
        else{
            query = "INSERT INTO GC_tags (title) VALUES (?)";
            QVector<QString> data = {addedTagText};
            UpdDelAddDataToDB(query, data);
            setTagsTableModel(*ui->tableView_problemTags_onCreateProblem);

            currentTags.insert(addedTagText);
            addTagsToProblem(*ui->tableView_problemTags_onCreateProblem, currentTags, *ui->label_addedTags_onCreateProblem);
        }
        loadingWindow.close();
    }

    else if(addedTagText.size()>20){
        callErrorBox(ERROR_TITLE, "Длина тега не должна превышать 20 символов, включая пробелы");
        ui->lineEdit_addTag_onCreateProblem->clear();
    }
    else
        callErrorBox(ERROR_TITLE, INCORRECT_DATA_ERROR);
}


void MainWindow::on_pushButton_createProblem_onAdminPanel_clicked()
{
    setTagsTableModel(*ui->tableView_problemTags_onCreateProblem);
    ui->lineEdit_addTag_onCreateProblem->clear();
    ui->pushButton_createProblem_onNewProblem->setToolTip(CREATE_PROBLEM_BUTTON_HINT);
    movePage(4);
}



