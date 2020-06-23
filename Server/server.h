#ifndef SERVER_H
#define SERVER_H

#include <QMainWindow>
#include <QtNetwork>
#include <QMessageBox>
#include <QKeyEvent>
#include <vector>
using namespace std;
//#include <QDebug>
//#include <QDateTime>
//#include <QSqlError>
//#include <QSqlDatabase>
//#include <QSqlQuery>

QT_BEGIN_NAMESPACE
namespace Ui { class Server; }
QT_END_NAMESPACE

class Server : public QMainWindow
{
    Q_OBJECT

public:
    Server(QWidget *parent = nullptr);
    ~Server();

private:
    Ui::Server *ui;

    QTcpServer *tcpServer;

    struct Client
    {
        QTcpSocket* tcpSocket;
        QString name;
    };

    vector<Client> ClientArr;
    void update_member_list();// 更新用户列表

private slots:
    // 定义函数，在cpp里实现。
//    void on_stopButton_clicked();
    void acceptConnection();
    void sendMessage();
//    void displayError(QAbstractSocket::SocketError);
    void receiveMessage();
//    void saveMessage(QString , QString , QString);
    void keyReleaseEvent(QKeyEvent *event);

    void on_pushButton_Fuckoff_clicked();
};
#endif // SERVER_H
