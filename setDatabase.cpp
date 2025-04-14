#include "mainwindow.h"


void MainWindow::setDatabase()
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(DB_hostName);
    db.setPort(DB_port);
    db.setUserName(DB_userName);
    db.setPassword(DB_password);
    db.setDatabaseName(DB_name);
}
