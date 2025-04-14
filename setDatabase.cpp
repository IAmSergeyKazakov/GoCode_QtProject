#include "mainwindow.h"


void MainWindow::setDatabase()
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("92.53.96.245");
    db.setPort(3306);
    db.setUserName("cf45733_kazak");
    db.setPassword("RBBT9SWh");
    db.setDatabaseName("cf45733_kazak");
}
