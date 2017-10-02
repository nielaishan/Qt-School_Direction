#include "mainwindow.h"
#include <QApplication>
#include "mysqldb.h"
using namespace  std;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MysqlDB mysql;
    if (!mysql.MysqlConnect())
        return -1;
    //mysql.CreatePath();
    MainWindow w;
    w.show();
    return a.exec();
}
