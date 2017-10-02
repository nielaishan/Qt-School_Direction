#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QTcpSocket>
#include <QTcpServer>
#include <QDebug>
#include <QString>

class TcpClient
{
public:
    TcpClient();
    ~TcpClient();
    void SendMsg(const QString& Msg);
    void BuildTcpConnect();

private:
    bool Status;
    QTcpSocket *Tcpsocket;

};

#endif // TCPCLIENT_H
