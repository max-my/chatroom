#include "server.h"
#include "ui_server.h"

Server::Server(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Server)
{
    ui->setupUi(this);

    tcpServer = new QTcpServer();
    if (!tcpServer->listen(QHostAddress::Any, 7777)) {
            qDebug() << tcpServer->errorString();
            close();
        }
    tcpSocketConnection = NULL;
    // 连接信号newConnection
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(acceptConnection()));
    // 将发送按钮和sendMessage函数关联起来
    connect(ui->pbSend,SIGNAL(clicked(bool)),this,SLOT(sendMessage()));
}

Server::~Server()
{
    delete ui;
}

// 连接信号
void Server::acceptConnection()
{
    // 调用nextPendingConnection获取连接进来的socket
    tcpSocketConnection = tcpServer->nextPendingConnection();
    // 根据不同的信号将tcpSocketConnection和相关函数关联
    connect(tcpSocketConnection,SIGNAL(disconnected()),this,SLOT(deleteLater()));
    connect(tcpSocketConnection,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));
    // 接受Client发来的消息，readyRead()准备读取信号，异步读取数据。
    connect(tcpSocketConnection, SIGNAL(readyRead()), this, SLOT(receiveMessage()));
}


// 关闭服务器，断开连接。
//void Server::on_stopButton_clicked()
//{
//    tcpSocketConnection->abort();
//    QMessageBox::about(NULL,"Connection","服务器关闭，连接终止。");
//}


// 向Client发送消息
void Server::sendMessage()
{
    //获取 输入框 里所输入的信息。
    QString temp_str = ui->textEdit_input->toPlainText();
    //获取当前时间
    QDateTime time = QDateTime::currentDateTime();
    QString nowtime = time.toString("yyyy-MM-dd hh:mm:ss");

    //显示在Server的消息记录里
    ui->textEdit_log->append(nowtime + "    Server:");
    ui->textEdit_log->append("    " + temp_str);

    QString str_msg = "server:" + temp_str;
    tcpSocketConnection->write(str_msg.toUtf8());
    // 将这条内容的有关信息存储到mysql。
//    saveMessage(nowtime, "Server", str);
}

// 存储到mysql
//void Server::saveMessage(QString time, QString user, QString content)
//{
//	// 连接并打开mysql
//    QSqlDatabase dataBase=QSqlDatabase::addDatabase("QMYSQL");
//    dataBase.setHostName("localhost");
//    dataBase.setUserName("root");
//    dataBase.setPassword("123456");
//    dataBase.setDatabaseName("ly");
//    dataBase.open();
//    QSqlQuery query(dataBase);
//    QString sql=QString("select *from chat");
//    query.exec(sql);
//    if(query.numRowsAffected() == 0)
//    {
//		// 将信息insert到chat表里。
//          QString savesql = QString("INSERT INTO chat(time, user, content)");
//          savesql += QString(" VALUES('%1','%2','%3')").arg(time).arg(user).arg(content);
//     }
//}


// 接收从Client发送来的消息。
void Server::receiveMessage()
{
    QDateTime time = QDateTime::currentDateTime();
    QString nowtime = time.toString("yyyy-MM-dd hh:mm:ss");
    // 使用readAll函数读取所有信息
    QString temp_str = tcpSocketConnection->readAll();
    int temp_pos = temp_str.indexOf(":");
    QString temp_name = temp_str.left(temp_pos);
    QString temp_msg = temp_str.mid(temp_pos+1);

    ui->textEdit_log->append(nowtime + "    " + temp_name);
    ui->textEdit_log->append("    " + temp_msg);
}


// 异常信息
void Server::displayError(QAbstractSocket::SocketError)
{
     qDebug() << tcpSocketConnection->errorString();
}
