#ifndef CLIENT_H
#define CLIENT_H

#include <QMainWindow>
#include <QtNetwork>
#include <QMessageBox>
#include <QKeyEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class Client; }
QT_END_NAMESPACE

class Client : public QMainWindow
{
    Q_OBJECT

public:
    Client(QWidget *parent = nullptr);
    ~Client();

private slots:
    void on_connectButton_clicked();
    void receiveMessage();
    void displayError(QAbstractSocket::SocketError);
    void sendMessage();
//    bool check(QString ,QString );
//    void on_logonbutton_clicked();
//    void saveMessage(QString , QString , QString);

    void on_pbPaita_clicked();

    void on_pbReturn_clicked();

    void keyReleaseEvent(QKeyEvent *event);

private:
    Ui::Client *ui;

    QTcpSocket *tcpSocket;

    bool is_connect;// 连接服务器标志
    QString str_name;// 客户昵称
};
#endif // CLIENT_H
