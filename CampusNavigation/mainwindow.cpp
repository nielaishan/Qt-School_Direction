#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    webview = new QWebEngineView;
    DrawWidget();
    GetLaLoXY();
    QWebEnginePage *page = new QWebEnginePage(this);
    webview->setPage(page);
    QUrl url;
    url.setUrl("qrc:/index.html");
    webview->load(url);
    webview->show();
    //网页交互
    QWebChannel *channel = new QWebChannel(this);
    channel->registerObject(QString("content"),this);
    page->setWebChannel(channel);
    //与服务端建立连接
    BuildTcpConnect();
    //信号与槽
    connect(QueryIntroButton, SIGNAL(clicked()),
            this, SLOT(IntroHandler()));
    connect(queryButton, SIGNAL(clicked()),
            this, SLOT(ShowWidget()));
    connect(ThroughButton, SIGNAL(clicked()),
            this, SLOT(ThroughInfoHandler()));
}

void MainWindow::BuildTcpConnect() {
    Tcpsocket = new QTcpSocket(this);
    Tcpsocket->abort();
    Tcpsocket->connectToHost(QHostAddress::LocalHost,6666);
    connect(Tcpsocket,SIGNAL(readyRead()),
            this, SLOT(dataReceived()));
    Tcpsocket->connectToHost("localhost",6666);
}

MainWindow::~MainWindow()
{
    source->stopUpdates();
    Tcpsocket->disconnect();
    this->disconnect();
}

//窗口的建立
void MainWindow::DrawWidget() {
    widget = new QWidget(this);
    LeftWidget = new QWidget;
    RightWidget = new QWidget;
    LeftWidget->setStyleSheet("border:2px groove gray;border-radius:10px;");
    FromLine = new QLineEdit;
    FromLine->setPlaceholderText("From");
    FromLine->setStyleSheet("background-color:rgb(250,250,250);border:2px groove gray;border-radius:10px;padding:4px 10px;");
    ToLine = new QLineEdit;
    ToLine->setPlaceholderText("To");
    ToLine->setStyleSheet("background-color:rgb(250,250,250);border:2px groove gray;border-radius:10px;padding:4px 10px;");
    InfoLine = new QLineEdit;
    InfoLine->setPlaceholderText("了解的景点...");
    InfoLine->setStyleSheet("background-color:rgb(250,250,250);border:2px groove gray;border-radius:10px;padding:4px 10px;");
    queryButton = new QPushButton;
    queryButton->setText("查询");
    queryButton->setStyleSheet("background-color:rgb(250,250,250);border:2px groove gray;border-radius:10px;padding:3px 8px;");
    QueryIntroButton = new QPushButton;
    QueryIntroButton->setText("简介查询");
    QueryIntroButton->setStyleSheet("background-color:rgb(250,250,250);border:2px groove gray;border-radius:10px;padding:3px 8px;");
    ThroughButton = new QPushButton;
    ThroughButton->setText("途径");
    ThroughButton->setStyleSheet("background-color:rgb(250,250,250);border:2px groove gray;border-radius:10px;padding:3px 8px;");
    //排版
    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(FromLine);
    topLayout->addWidget(ToLine);
    topLayout->addWidget(InfoLine);
    topLayout->addWidget(QueryIntroButton);
    topLayout->addWidget(ThroughButton);
    topLayout->addWidget(queryButton);
    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(webview);
    QVBoxLayout *leftLayout = new QVBoxLayout(LeftWidget);
    leftLayout->addLayout(topLayout);
    leftLayout->addLayout(bottomLayout);

    //右窗口的设计
    RightListWidget = new QListWidget;
    QVBoxLayout *rightLayout = new QVBoxLayout;
    label = new QLabel;
    RightWidget->hide();
    RightWidget->resize(300, 700);
    QFont font("Microsoft YaHei", 16, 75);
    RightWidget->setFont(font);
    RightWidget->setStyleSheet("background-color:rgb(250,250,250);");

    //插入右窗口
    ThroughText = new QTextEdit;
    ThroughText->setPlaceholderText(tr("途径的景点，请用空格隔开"));
    rightLayout->addWidget(label);
    rightLayout->addWidget(RightListWidget);
    rightLayout->addWidget(ThroughText);
    RightWidget->setLayout(rightLayout);
    mainLayout = new QHBoxLayout(widget);
    mainLayout->addWidget(LeftWidget);
    mainLayout->addWidget(RightWidget);
    mainLayout->setSpacing(0);
    //主窗口设置
    widget->setLayout(mainLayout);
    this->setCentralWidget(widget);
    //this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setStyleSheet("background-color:rgb(245,245,245);"); //设置主窗口样式
    this->resize(800, 700);
    this->setWindowTitle(tr("校园导航系统"));
}
//简介的槽函数
void MainWindow::IntroHandler() {
    if (InfoLine->text().length()) {
        QString msg = "2";
        msg += InfoLine->text();
        label->setText(tr("景点信息"));
        SendMsg(msg);
        label->show();
        RightWidget->show();
        RightListWidget->show();
        ThroughText->hide();
    }
    else {
        return ;
    }
}
//指定景点的槽函数
void MainWindow::ThroughInfoHandler() {
    qDebug()<<"Text";
    label->setText(tr("请写入想要去的景点"));
    RightWidget->show();
    ThroughText->show();
    RightListWidget->hide();
}
//查询的槽函数
void MainWindow::ShowWidget() {  //显示右窗口

    if (!ThroughText->toPlainText().length() && ToLine->text().length() && FromLine->text().length()) { //显示窗口并
        QString Msg="1"+FromLine->text()+" "+ToLine->text();
        SendMsg(Msg);
        label->setText(tr("推荐路径"));
        label->show();
        RightWidget->show();
        RightListWidget->show();
        ThroughText->hide();
        RightListWidget->clear();
    }
    else if (ThroughText->toPlainText().length()) {
        label->setText(tr("推荐路径"));
        if (FromLine->text().length()) {
            QString Msg="31 "+FromLine->text()+" "+ThroughText->toPlainText();
            SendMsg(Msg);
        }
        else {
            while (!X.length() || !Y.length());
            QString Msg="32 "+X+'#'+Y+" "+ThroughText->toPlainText();
            SendMsg(Msg);
        }
        RightWidget->show();
        RightListWidget->show();
        ThroughText->hide();
        RightListWidget->clear();
    }
    else
        RightWidget->hide();
    ThroughText->clear();
}

void MainWindow::TransformLaLo(QString msg, double &f_la, double &f_lo, double &t_la, double &t_lo) {
    QString tmp="";
    int i=0;
    for (; i<msg.size(); i++) {
        if (msg[i] == ' ') {
            i++;
            break;
        }
        tmp += msg[i];
    }
    f_la = tmp.toDouble();
    tmp.clear();
    for (; i<msg.size(); i++) {
        if (msg[i] == ' ') {
            i++;
            break;
        }
        tmp += msg[i];
    }
    f_lo = tmp.toDouble();
    tmp.clear();
    for (; i<msg.size(); i++) {
        if (msg[i] == ' ') {
            i++;
            break;
        }
        tmp += msg[i];
    }
    t_la = tmp.toDouble();
    tmp.clear();
    for (; i<msg.size(); i++) {
        if (msg[i] == ' ') {
            i++;
            break;
        }
        tmp += msg[i];
    }
    t_lo = tmp.toDouble();
    tmp.clear();
    return ;
}

void MainWindow::SendMsg(QString Msg) {
    QByteArray message;
    QDataStream out(&message,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out<<Msg;
    Tcpsocket->write(message);
}

void MainWindow::dataReceived()
{
    QDataStream in(Tcpsocket);
    in.setVersion(QDataStream::Qt_5_4);
    QString msg;
    in >> msg;
    if (msg[0] == '1') {
        QStringList SList = msg.split('#');
        RightListWidget->addItem(SList.at(1));
        qDebug()<<SList.at(0);
        QString LaLo = SList.at(0).mid(1);
        double f_la, f_lo, t_la, t_lo;
        TransformLaLo(LaLo, f_la, f_lo, t_la, t_lo);
        SendPath(f_la, f_lo, t_la, t_lo);
    }
    else if (msg[0] == '2') {
        QString p_info = msg.mid(1);
        RightListWidget->clear();
        RightListWidget->addItem(p_info);
    }
    else if (msg[0] == '3') {
        QString p_info = msg.mid(1);
        QStringList plist = p_info.split('#');
        RightListWidget->addItem(plist.at(0));
        QString LaLo = plist.at(1);
        qDebug()<<LaLo;
        double f_la, f_lo, t_la, t_lo;
        TransformLaLo(LaLo, f_la, f_lo, t_la, t_lo);
        cout<<f_la<<' '<<f_lo<<' '<<t_la<<' '<<t_lo<<endl;
        SendPath(f_la, f_lo, t_la, t_lo);
    }
}

void MainWindow::GetLaLoXY() {   //得到经纬度
    source = QGeoPositionInfoSource::createDefaultSource(this);
    if (source) {
        connect(source, SIGNAL(positionUpdated(QGeoPositionInfo)),
                this, SLOT(positionUpdated(QGeoPositionInfo)));  //can be used to specify how often these updates should be emitted
        source->setPreferredPositioningMethods(QGeoPositionInfoSource::NonSatellitePositioningMethods);
        source->setUpdateInterval(1000);//can be used to specify how often these updates should be emitted
        source->startUpdates();
    }
}

//位置更新
void MainWindow::positionUpdated(const QGeoPositionInfo &info) {
    coordinate = info.coordinate();
    double la, lo;
    Transform(coordinate.latitude(), coordinate.longitude(), &la, &lo);
    X = QString::number(lo, '.', 6);
    Y = QString::number(la, '.', 6);
    SendLaLoXY(X, Y);
}
//以下函数GPS转换为高德地图的经纬度。
double MainWindow::transformlat(const double& x,const double& y)
{
    double ret = -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y +
        0.2 * sqrt(fabs(x));
    ret += (20.0 * sin(6.0 * x * PI) + 20.0 * sin(2.0 * x * PI)) * 2.0 / 3.0;
    ret += (20.0 * sin(y * PI) + 40.0 * sin(y / 3.0 * PI)) * 2.0 / 3.0;
    ret += (160.0 * sin(y / 12.0 * PI) + 320 * sin(y * PI / 30.0)) * 2.0 / 3.0;
    return ret;
}
double MainWindow::transformlon(const double& x, const double& y)
{
    double ret =
        300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y + 0.1 * sqrt(fabs(x));
    ret += (20.0 * sin(6.0 * x * PI) + 20.0 * sin(2.0 * x * PI)) * 2.0 / 3.0;
    ret += (20.0 * sin(x * PI) + 40.0 * sin(x / 3.0 * PI)) * 2.0 / 3.0;
    ret +=
        (150.0 * sin(x/12.0 * PI) + 300.0 * sin(x / 30.0 * PI)) * 2.0 / 3.0;
    return ret;
}
void MainWindow::Transform(double wglat, double wglon, double *mglat, double *mglon)
{
    const double a = 6378245.0;
    const double ee = 0.00669342162296594323;
    double dlat = transformlat(wglon - 105.0, wglat - 35.0);
    double dlon = transformlon(wglon - 105.0, wglat - 35.0);
    double radlat = wglat / 180.0 * PI;
    double magic = sin(radlat);
    magic = 1 - ee * magic * magic;
    double sqrtmagic = sqrt(magic);
    dlat = (dlat * 180.0) / ((a * (1 - ee)) / (magic * sqrtmagic) * PI);
    dlon = (dlon * 180.0) / (a / sqrtmagic * cos(radlat) * PI);
    *mglat = wglat + dlat;
    *mglon = wglon + dlon;
}
