#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPaintEvent>
#include <QDebug>
#include <QFont>
#include <QTextEdit>
#include <QListWidget>
//插入Html，所需的头文件
#include <QWebChannel>
#include <QWebEngineView>
#include <QUrl>
#include <QUrlQuery>

//定位所需的头文件
#include <QGeoPositionInfoSource>
#include <QGeoCoordinate>
#include <QGeoPositionInfo>
#include <cmath>
//TCP连接
#include <QTcpSocket>
#include <QHostAddress>
#include <iostream>
#include <QStringList>
static double x_pi = 3.14159265358979324 * 3000.0 / 180.0;
static double PI = 3.14159265358979324;
using namespace  std;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void DrawWidget();
    void GetLaLoXY();
    double transformlon(const double &x, const double &y);
    double transformlat(const double &x, const double &y);
    void Transform(double , double , double *, double*);
    void BuildTcpConnect();
    void SendMsg(QString Msg);
    void TransformLaLo(QString, double &, double &, double &, double &);
signals:
    void SendLaLoXY(const QString &X, const QString &Y);
    void SendPath(const double&, const double& ,const double&,const double&);
private slots:
    void ShowWidget();
    void IntroHandler();
    void ThroughInfoHandler();
    void dataReceived();
    void positionUpdated(const QGeoPositionInfo &info);
private:
    QPushButton *queryButton;
    QPushButton *QueryIntroButton;
    QPushButton *ThroughButton;
    QLineEdit *FromLine;
    QLineEdit *ToLine;
    QLineEdit *InfoLine;
    QTextEdit *ThroughText;
    QLabel *label;
    QGeoPositionInfoSource *source;
    QGeoCoordinate coordinate;
    QWidget *widget;

    QListWidget *RightListWidget;
    QWidget *LeftWidget;
    QWidget *RightWidget;
    QWebEngineView *webview;
    QHBoxLayout *mainLayout;
    //TCP
    bool Status;
    int port;
    QString X, Y;
    QHostAddress *ServerIP;
    QTcpSocket *Tcpsocket;
    QString ReceivedMsg;
    QVector<QLabel *>vec;
};

#endif // MAINWINDOW_H
