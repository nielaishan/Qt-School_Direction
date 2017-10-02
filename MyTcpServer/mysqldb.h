#ifndef MYSQLDB_H
#define MYSQLDB_H

#include <QSqlDatabase>
#include <iostream>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>
#include <QPluginLoader>
#include <QDebug>
#include <QSqlTableModel>
#include <QString>
#include <vector>
#include <cstring>
#include <string>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <map>
#include <queue>
#include <sstream>
using namespace std;
class MysqlDB
{
public:
    typedef long long LL;
    typedef pair<int, double> P;
    double inf = 0x3f3f3f3f3f;
    MysqlDB();
    ~MysqlDB();
    bool MysqlConnect();
    void CreatePath();
    void DFS(int, int ,int, QString, double);
    QString GetLaLo(string from, string to);
    QString GetPathandLalo(string from, string to);
    QString GetInfo(QString message);
    QString GetPlace(QString name);
    QString GetPlace(int id);
    QString GetStart(QString LaLo);
    void GetPath();
    double Dijkstra(int s, int t, int *path, int n);
    QString Search_Mul_place(QStringList list, QString S);
private:
    QSqlDatabase data;
    vector<pair<int,double>>g[100];
    int N;
    bool vis[100];
    map<int, string>ma;
    vector<int>res;
};
#endif //MYSQLDB_H
