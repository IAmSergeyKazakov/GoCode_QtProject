#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::on_pushButton_userAuth_onGreet_clicked()
{
    QString name = ui->lineEdit_name_onGreet->text();
    QString password = ui->lineEdit_password_onGreet->text();
    QString realPassword;
    bool isAdminAccount;


    if (db.open())
    {
        QString query = "SELECT password FROM GC_users WHERE name = :name";
        realPassword = getValueFromDB(query,{name});

        query = "SELECT id FROM GC_users WHERE name = :name";
        global_userID = getValueFromDB(query, {name});

        if (hashPassword(password) == realPassword && password.size()!=0 && name.size()!=0){
            setLoadingWindow();
            global_userName = name;
            isAdminAccount = isAdminAcc(name);

            if(isAdminAccount){
                movePage(3);
            }

            else
            {

                setProblemsList(*ui->tableView_problemsList_onMain);
                setTableViewVisibility(*ui->tableView_problemsList_onMain, *ui->label_emptyData_onMain);

                QString query = "SELECT title FROM GC_tags";
                QVector<QString>allTags = getSomeValuesFromDB(query, {});
                ui->comboBox_problemTags_onMain->addItems(allTags.toList());
                movePage(2);

            }

        }
        else if (password != realPassword){
            callErrorBox(ERROR_TITLE, "Неверное имя пользователя или пароль");
        }
        else{
            callErrorBox(ERROR_TITLE, INCORRECT_DATA_ERROR);
        }

    }
    else{
        callErrorBox(ERROR_TITLE, DATABASE_ERROR);
    }

    loadingWindow.close();
}
