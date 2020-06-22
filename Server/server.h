#ifndef SERVER_H
#define SERVER_H

#include <QMainWindow>
#include <QtNetwork>
#include <QMessageBox>
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

private slots:
    // 定义函数，在cpp里实现。
//    void on_stopButton_clicked();
    void acceptConnection();
    void sendMessage();
//    void displayError(QAbstractSocket::SocketError);
    void receiveMessage();
//    void saveMessage(QString , QString , QString);

private:
    Ui::Server *ui;

    QTcpServer *tcpServer;
    vector<QTcpSocket*> SocketArr;


};
#endif // SERVER_H
