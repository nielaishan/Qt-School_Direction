#include "mysqldb.h"
#include <iostream>
using namespace std;
MysqlDB::MysqlDB(){
}

MysqlDB::~MysqlDB(){
}
//数据库的连接
bool MysqlDB::MysqlConnect() {
    QPluginLoader loader;
    // MySQL 驱动插件的路径
    loader.setFileName("/Users/nielaishan/Qt5.9.1/5.9.1/clang_64/plugins/sqldrivers/libqsqlmysql.dylib");
    data = QSqlDatabase::addDatabase("QMYSQL");
    data.setHostName("127.0.0.1");
    data.setDatabaseName("mydata");
    data.setPort(3306);
    data.setUserName("root");
    data.setPassword("wyzdz");
    if(data.open()) {
        cout<<"Hi mysql!连接"<<endl;
        return true;
    }
    else {
        cout<<"失败"<<endl;
        return false;
    }
}

//创建路径
void MysqlDB::CreatePath()
{
    QSqlQuery query;
    query.exec("select * from path");
    N = 0;
    while (query.next()) {
        int u = query.value(0).toInt();
        int v = query.value(1).toInt();
        N = max(N, max(u, v));
        double w = query.value(2).toDouble();
        cout<<u<<' '<<v<<' '<<w<<endl;
        g[u].push_back(make_pair(v, w));
        g[v].push_back(make_pair(u, w));
    }
    cout<<N<<endl;
    for (int i=1; i<=N; i++) {
        for (int j=1; j<=N; j++) {
            if (i == j)
                continue;
            memset(vis, false, sizeof(vis));
            query.prepare("select * from place where p_id=:p_id");
            query.bindValue(":p_id", i);
            query.exec();
            QString t;
            while (query.next()) {
                t = query.value(0).toString();
            }
            ma[i] = t.toStdString();
            vis[i] = true;
            DFS(i, i, j, t, 0.0);
        }
    }
}
//深度搜索图
void MysqlDB::DFS(int s, int u, int t, QString str, double dis) {
    if (u == t) {
        str = str;
        QSqlQuery query;
        query.prepare("insert into edge(e_from, e_to, e_pa, e_dis) value(:from,:to,:pa,:dis)");
        //qDebug()<<s<<" "<<t<<" "<<str<<" "<<dis;
        cout<<ma[s].c_str()<<' '<<ma[t].c_str()<<' '<<str.toStdString().c_str()<<endl;
        query.bindValue(":from", ma[s].c_str());
        query.bindValue(":to", ma[t].c_str());
        query.bindValue(":pa", str.toStdString().c_str());
        query.bindValue(":dis", dis);
        query.exec();
        return ;
    }
    for (size_t i=0; i<g[u].size(); i++) {
        int v = g[u][i].first;
        if (!vis[v]) {
            QSqlQuery query;
            query.prepare("select p_name from place where p_id=:id");
            query.bindValue(":id", v);
            query.exec();
            QString tmp;
            QString place;
            tmp = "->";
            while (query.next()) {
                place = query.value(0).toString();
                tmp += place;
            }
            vis[v] = true;
            ma[v] = place.toStdString();
            DFS(s, v, t, str+tmp, dis+g[u][i].second);
            vis[v] = false;
        }
    }
    return ;
}
//发送所有路径和经纬度
QString MysqlDB::GetPathandLalo(string from, string to) {
    QSqlQuery query;
    query.prepare("select * from edge where e_from=:from and e_to=:to order by e_dis");
    query.bindValue(":from", from.c_str());
    query.bindValue(":to", to.c_str());
    query.exec();
    QString msg="1";
    msg += GetLaLo(from, to)+'#';
    int cnt = 0;
    while(query.next()) {
        cnt++;
        if (cnt <= 10)
            msg += query.value(3).toString()+'\n';
    }
    qDebug()<<"msg: "<<msg<<endl;
    return msg;
}
//发送from的经纬度和to的经纬度
QString MysqlDB::GetLaLo(string from, string to) {
    QSqlQuery query;
    query.prepare("select p_la, p_lo from place where p_name=:name");
    query.bindValue(":name", from.c_str());
    bool flag = query.exec();
    QString f_la=0, f_lo=0, t_la=0, t_lo=0;
    if (flag) {
        while (query.next()) {
            f_la = query.value(0).toString();
            f_lo = query.value(1).toString();
        }
    }
    else
        return "error";
    query.prepare("select p_la, p_lo from place where p_name=:name");
    query.bindValue(":name", to.c_str());
    flag = query.exec();
    if (flag) {
        while (query.next()) {
            t_la = query.value(0).toString();
            t_lo = query.value(1).toString();
        }
        QString msg;
        msg = f_la+' '+f_lo+' '+t_la+' '+t_lo;
        return msg;
    }
    else
        return "error";
}
//得到简介
QString MysqlDB::GetInfo(QString message) {
    QSqlQuery query;
    query.prepare("select p_info from place where p_name=:name");
    QString p_name = message.mid(1);
    query.bindValue(":name", p_name);
    bool flag = query.exec();
    QString p_info="2";
    if (flag) {
        while (query.next())
            p_info += query.value(0).toString();
        return p_info;
    }
    return p_info+"地点有误";
}
//得到Place的信息
QString MysqlDB::GetPlace(int id) {
    QSqlQuery query;
    query.prepare("select * from place where p_id=:id");
    query.bindValue(":id", id);
    bool flag=query.exec();
    QString place;
    if (flag) {
        while (query.next())
            place = query.value(0).toString()+' '+query.value(1).toString()+' '
                    +query.value(2).toString()+' '+query.value(3).toString()+' '
                    +query.value(4).toString();
        return place;
    }
    return "error";
}
//根据经纬度查找当前最近的景点
QString MysqlDB::GetStart(QString LaLo)
{
    QStringList list = LaLo.split('#');
    double la = list.at(0).toDouble();
    double lo = list.at(1).toDouble();
    QSqlQuery query;
    bool flag = query.exec("select * from place");
    double minn = 100000000.0;
    QString p_name;
    if(flag) {
        while (query.next()) {
            double x = query.value(2).toDouble();
            double y = query.value(3).toDouble();
            double res = (x-la)*(x-la)+(y-lo)*(y-lo);
            if (res < minn) {
                minn = res;
                p_name = query.value(0).toString();
            }
        }
        return p_name;
    }
    return "error";
}
//根据名字查找Place的信息
QString MysqlDB::GetPlace(QString name) {
    QSqlQuery query;
    query.prepare("select * from place where p_name=:name");
    query.bindValue(":name", name);
    bool flag=query.exec();
    QString place;
    if (flag) {
        while (query.next())
            place = query.value(0).toString()+' '+query.value(1).toString()+' '
                    +query.value(2).toString()+' '+query.value(3).toString()+' '
                    +query.value(4).toString();
        return place;
    }
    return "error";
}

//提取数据，建边
void MysqlDB::GetPath() {
    QSqlQuery query;
    query.exec("select * from path");
    while (query.next()) {
        int u=query.value(0).toInt();
        int v=query.value(1).toInt();
        double dis = query.value(2).toDouble();
        g[u].push_back(make_pair(v, dis));
        g[v].push_back(make_pair(u, dis));
    }
}
//单源最短路径
double MysqlDB::Dijkstra(int s, int t, int *path, int n)
{
    int min_num;
    double dis[50], min;
    int vis[50];
    memset(vis, 0, sizeof(vis));
    for(int i=1;i<=n;i++)
        dis[i]=inf;
    for (int i=0; i<g[s].size(); i++)
        dis[g[s][i].first] = g[s][i].second, path[g[s][i].first] = s;
    dis[s]=0;
    vis[s]=1;
    for(int i=1;i<n;i++)
    {
        min=inf;
        for(int j=1;j<=n;j++)
        {
            if(!vis[j]&&dis[j]<min)
            {
                min=dis[j];
                min_num=j;
            }
        }
        if (vis[min_num])
            break;
        vis[min_num]=1;
        for(int j=0;j<g[min_num].size();j++)
        {
            int v=g[min_num][j].first;
            if(dis[v]>min+g[min_num][j].second)
            {
                path[v]=min_num;//path[j]记录d[j]暂时最短路径的最后一个中途节点min_num，表明d[j]最后一段从节点min_num到节点j
                dis[v]=min+g[min_num][j].second;
            }
        }
    }
    printf("dis: %.5f\n", dis[t]);
    return dis[t];
}

//经过指定点的最短路径
QString MysqlDB::Search_Mul_place(QStringList list, QString S) {
    GetPath(); //得到所有边的信息
    //得到起点的信息
    QString place = GetPlace(S);
    QStringList plist = place.split(' ');
    int s=plist.at(1).toInt();
    int n=0;
    int a[100];
    vector<int>ve;
    for (int i=2; i<list.size(); i++) {
        QString place = GetPlace(list.at(i));
        plist = place.split(' ');
        ve.push_back(plist.at(1).toInt());
    }
    sort(ve.begin(), ve.end());
    ve.erase(unique(ve.begin(), ve.end()), ve.end());
    n = (int)ve.size();
    for (size_t i=0; i<ve.size(); i++)
        a[i] = ve[i];
    double dis = 1000000.0;
    int path[100];
    string pa;
    do {
        cout<<"A: "<<endl;
       for (int i=0; i<n; i++)
           cout<<a[i]<<' ';
       cout<<endl;
       string tmp = "";
       double res = 0;
       for (int i=0; i<n; i++) {
           if (i == 0) {
               memset(path, -1, sizeof(path));//路径初始化
               qDebug()<<"do while";
               //求两点之间的最短路径
               res += Dijkstra(s, a[i], path, 13);
               int p = a[i];
               string t="";
               while (p != -1) {
                   int tt = p;
                   string temp="";
                   while (tt)
                       temp += tt%10+'0', tt/=10;
                   t += " "+temp;
                   p=path[p];
               }
               reverse(t.begin(), t.end());
               tmp += t;
           }
           else {
               memset(path, -1, sizeof(path));
               res += Dijkstra(a[i-1], a[i], path, 13);
               int p = a[i];
               string t="";
               while (path[p] != -1) {
                   int tt = p;
                   string temp="";
                   while (tt)
                       temp += tt%10+'0', tt/=10;
                   t += " "+temp;
                   p=path[p];
               }
               //反转路径
               reverse(t.begin(), t.end());
               tmp += t;
           }
       }
       if (dis > res) {
           dis = res;
           pa = tmp;
       }
    }while (next_permutation(a, a+n));
    QString msg=QString::fromStdString(pa);
    plist = msg.split(" ");
    QString Path="";
    QString Start, End;
    for (int i=0; i<plist.size(); i++) {
        if (i == 0 && plist.at(i).length()) {
            int id = plist.at(i).toInt();
            place = GetPlace(id);
            QStringList List = place.split(" ");
            Path = List.at(0);
            Start = Path;
        }
        else if (plist.at(i).length()) {
            Path+="->";
            int id = plist.at(i).toInt();
            place = GetPlace(id);
            QStringList List = place.split(" ");
            Path += List.at(0);
            End = List.at(0);
        }
    }
    Path = Path+'#'+GetLaLo(Start.toStdString(), End.toStdString());
    return Path;
}
