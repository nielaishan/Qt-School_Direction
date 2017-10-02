#include "tcpclient.h"

TcpClient::TcpClient()
{
    Tcpsocket = new QTcpSocket;
    Status = false;
}

void TcpClient::BuildTcpConnect() {

}

void
