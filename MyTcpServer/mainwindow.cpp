#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    mysql = new MysqlDB;
    DrawWidget();
    m_tcpserver = new QTcpServer(this);
    m_tcpsocket = new QTcpSocket(this);
    //开始监听端口
    m_tcpserver->listen(QHostAddress::Any,6666);
    //新请求的，响应信号，调用函数
    connect(m_tcpserver,SIGNAL(newConnection()),
            this,SLOT(newConnect()));
    m_tcpserver->listen(QHostAddress::Any,6666);
}

MainWindow::~MainWindow()
{
    m_tcpserver->disconnect();
}

//主界面绘制
void MainWindow::DrawWidget() {
    QGroupBox *place = createPlaceView();
    QGroupBox *path = createPathView();
    QGridLayout *left = new QGridLayout;
    left->addWidget(place, 0, 0);
    left->addWidget(path,100, 0);
    left->setColumnMinimumWidth(0,500);
    RecevedLine = new QListWidget;
    mainLayout = new QHBoxLayout;
    CreateBtn = new QPushButton(tr("send"));

    QVBoxLayout *right = new QVBoxLayout;
    QLabel *label = new QLabel(tr("日志记录"));
    right->addWidget(label);
    right->addWidget(RecevedLine);
    right->addWidget(CreateBtn);

    mainLayout->addLayout(left);
    mainLayout->addLayout(right);
    widget = new QWidget;
    widget->setLayout(mainLayout);
    this->setCentralWidget(widget);
    createMenuBar();
    //widget->setStyleSheet("background-color:rgb(250,250,250);border:2px groove gray;border-radius:10px;"); //设置主窗口样式
    this->resize(1000, 800);
    this->setWindowTitle(tr("校园导航系统服务端"));
}

QGroupBox *MainWindow::createPlaceView()
{
    PlaceView = new QTableView;
    QSqlTableModel *PlaceModel = new QSqlTableModel(this);
    PlaceModel->setTable("place");
    PlaceModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    PlaceModel->select(); //选取整个表的所有行
    PlaceView->setModel(PlaceModel);
    PlaceView->setSortingEnabled(true);
    PlaceView->setSelectionMode(QAbstractItemView::SingleSelection);
    PlaceView->setShowGrid(false);
    PlaceView->setAlternatingRowColors(true);
    PlaceView->setColumnWidth(10, 80);
    QGroupBox *box = new QGroupBox(tr("Place"));
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(PlaceView);
    box->setLayout(layout);
    return box;
}

void MainWindow::createMenuBar() {
    QAction *addAction = new QAction(tr("添加"), this);
    QAction *deleteAction = new QAction(tr("删除"), this);
    QAction *quitAction = new QAction(tr("退出"), this);
    addAction->setShortcut(tr("Ctrl+A"));
    deleteAction->setShortcut(tr("Ctrl+D"));
    quitAction->setShortcut(tr("Ctrl+Q"));
    QMenu *fileMenu = this->menuBar()->addMenu(tr("操作菜单"));
    fileMenu->addAction(addAction);
    fileMenu->addSeparator();
    fileMenu->addAction(deleteAction);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);
}

QGroupBox *MainWindow::createPathView()
{
    PathView = new QTableView;
    QSqlTableModel *PathModel = new QSqlTableModel(this);
    PathModel->setTable("path");
    PathModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    PathModel->select(); //选取整个表的所有行
    PathView->setModel(PathModel);
    PathView->setSortingEnabled(true);
    PathView->setSelectionMode(QAbstractItemView::SingleSelection);
    PathView->setShowGrid(false);
    PathView->setAlternatingRowColors(true);
    QGroupBox *box = new QGroupBox(tr("Path"));
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(PathView);
    box->setLayout(layout);
    return box;
}

void MainWindow::newConnect()
{
    m_tcpsocket = m_tcpserver->nextPendingConnection();
    connect(m_tcpsocket, SIGNAL(readyRead()),
            this, SLOT(readMessage()));
    connect(m_tcpsocket, SIGNAL(disconnected()),
                m_tcpsocket, SLOT(deleteLater()));
}

//从客户端读取数据
void MainWindow::readMessage()
{
    QDataStream in(m_tcpsocket);
    in.setVersion (QDataStream::Qt_5_4);
    in >> message;
    if (message[0] == '1') {//发送经纬度和所有路径
        string temp = message.toStdString();
        string from="", to="";
        int i;
        for (i=1; i<temp.size(); i++) {
            if (temp[i] == ' ')
               break;
            from+=temp[i];
        }
        i++;
        for (;i<temp.size(); i++)
            to += temp[i];
        QString info = mysql->GetPathandLalo(from, to);
        sendMessage(info);
    }
    else if (message[0] == '2'){//发送简介
        QString info = mysql->GetInfo(message);
        sendMessage(info);
    }
    else if (message[0] == '3') { //指定途径的路径
        QString info;
        if (message[1] == '1') {
            QStringList list = message.split(" ");
            QString s = list.at(1);
            qDebug()<<s;
            QString t = mysql->Search_Mul_place(list, s);
            info = "3"+t;
        }
        else if (message[1] == '2') {
            QStringList list = message.split(' ');
            QString s = mysql->GetStart(list.at(1));
            qDebug()<<s;
            QString t = mysql->Search_Mul_place(list, s);
            info ="3"+t;
        }
        qDebug()<<"info: "<<info;
        sendMessage(info);
    }
    RecevedLine->addItem(message);
}

//向客户端发送数据
void MainWindow::sendMessage(QString msg)
{
    QByteArray message;
    QDataStream out(&message,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out<<msg;
    m_tcpsocket->write(message);
}
