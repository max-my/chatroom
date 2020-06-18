#ifndef CLIENT_H
#define CLIENT_H

#include <QMainWindow>
#include <QtNetwork>
#include <QMessageBox>


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

private:
    Ui::Client *ui;

    QTcpSocket *tcpSocket;
};
#endif // CLIENT_H
