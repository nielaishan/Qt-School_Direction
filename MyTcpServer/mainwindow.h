#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTableView>
#include <QTextLine>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QGroupBox>
#include <QSqlTableModel>
#include <QSql>
#include <QLabel>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QHeaderView>
#include "mysqldb.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void DrawWidget();
protected:
    QGroupBox *createPlaceView();
    QGroupBox *createPathView();
    void createMenuBar();
    void SendPath(string, string);
    QString SendLaLo(string, string);
private:
    QString message;
    QString greeting;
    QTcpSocket *m_tcpsocket;
    QTcpServer *m_tcpserver;
    QTableView *PlaceView;
    QTableView *PathView;
    QListWidget *RecevedLine;
    QWidget *widget;
    QHBoxLayout *mainLayout;
    QPushButton *CreateBtn;
    MysqlDB *mysql;
private slots:
    void newConnect();
    void sendMessage(QString msg);
    void readMessage();
};

#endif // MAINWINDOW_H
