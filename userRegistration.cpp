#include "mainwindow.h"
#include "ui_mainwindow.h"



void MainWindow::on_pushButton_CreateAcc_onRegistration_clicked()
{
    QString name = ui->lineEdit_name_onRegistration->text();
    QString password = ui->lineEdit_password_onRegistration->text();
    QString confirmPassword = ui->lineEdit_confirmPassword_onRegistration->text();
    QSqlQuery addDataQuery(db);
    QString userID;

    if (db.open())
    {
        QString query = "SELECT name FROM GC_users WHERE name = :name";
        QString isAvailableName = getValueFromDB(query, {name});

        if (isAvailableName.size() == 0 && confirmPassword == password && confirmPassword.size()>0 && password.size()>0 && name.size()>0 && password.size()>=6)
        {
            setLoadingWindow();

            query = "INSERT INTO GC_users (name,password,isAdmin, creation_date) VALUES (?, ?, ?, ?)";
            QVector<QString> data = {name, hashPassword(password), "0",
                                     QDate::currentDate().toString(Qt::ISODate)};
            UpdDelAddDataToDB(query, data);

            query = "SELECT id FROM GC_users WHERE name = :name AND isAdmin = 0";
            userID = getValueFromDB(query, {name});

            callAcceptBox(ACCEPT_TITLE, "Вы успешно создали аккаунт " + name);
            loadingWindow.close();
            movePage(0);
        }
        else if (isAvailableName.size() != 0)
            callErrorBox(ERROR_TITLE, "Данное имя занято другим пользователем");
        else if (confirmPassword != password)
            callErrorBox(ERROR_TITLE, "Пароли не совпадают");
        else if (password.size()<6)
            callErrorBox(ERROR_TITLE, "Пароль должен состоять минимум из 6 символов");
        else
            callErrorBox(ERROR_TITLE, "Некорректные данные");

    }
    else
        callErrorBox(ERROR_TITLE, DATABASE_ERROR);

}


void MainWindow::on_pushButton_userRegistration_onGreet_clicked() {movePage(1);};


