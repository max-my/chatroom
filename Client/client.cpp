#include "client.h"
#include "ui_client.h"

Client::Client(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Client)
{
    ui->setupUi(this);

    tcpSocket = new QTcpSocket(this);
    // 关联登录按钮和函数，进行确认登录并连接到服务器
//    connect(ui->connectButton,SIGNAL(clicked()),this,SLOT(on_connectButton_clicked()));

    // 关联连接按钮和函数，连接到服务器
    connect(ui->pbConnect,SIGNAL(clicked()),this,SLOT(on_connectButton_clicked()));

    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError(QAbstractSocket::SocketError)));
    // 接受Server发来的消息，readyRead()准备读取信号，异步读取数据。
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(receiveMessage()));
    // 将发送按钮和sendMessage函数关联起来
    connect(ui->pbSend,SIGNAL(clicked(bool)),this,SLOT(sendMessage()));
}

Client::~Client()
{
    delete ui;
}

//// 连接数据库，判断所输入的用户名和密码是否正确
//bool Client::check(QString ID, QString PW)
//{
//    QSqlDatabase dataBase=QSqlDatabase::addDatabase("QMYSQL");
//    dataBase.setHostName("localhost");
//    dataBase.setUserName("vici");
//    dataBase.setPassword("123456");
//    dataBase.setDatabaseName("ly");
//    dataBase.open();
//    QSqlQuery showquery(dataBase);
//    QString showsql=QString("select *from user;");
//    showquery.exec(showsql);
//    if(showquery.numRowsAffected() != 0)
//    {
//        while(showquery.next())
//        {
//            if(showquery.value(0).toString() == ID && showquery.value(1).toString() == PW)
//                return true;
//        }
//    }
//    return false;
//}

//// 登录按钮，登录信息正确则连接到服务器
//void Client::on_connectButton_clicked()
//{
//    if(tcpSocket->state()!=QAbstractSocket::ConnectedState)
//    {
//        // 获取用户名和密码
//        QString ID = ui->IDLineEdit->text();
//        QString PW = ui->PWLineEdit->text();

//        // 检查
//        if(check(ID, PW))
//        {
//            tcpSocket->connectToHost("127.0.0.1", 7777);

//            if(tcpSocket->waitForConnected(10000))
//            {
//                QMessageBox::about(NULL, "Connection", "登录成功！");
//            }
//            else
//            {
//                QMessageBox::about(NULL,"Connection","登录失败！");
//            }
//        }

//    }
//    else
//        QMessageBox::information(NULL,"","已登录。");
//}


// 连接按钮
void Client::on_connectButton_clicked()
{
    tcpSocket->connectToHost("127.0.0.1", 7777);

    if(tcpSocket->waitForConnected(10000))
    {
        QMessageBox::about(NULL, "Connection", "登录成功！");
    }
    else
    {
        QMessageBox::about(NULL,"Connection","登录失败！");
    }
}

// 发送信息，并存储到数据库
void Client::sendMessage()
{
    QString str = ui->textEdit_input->toPlainText();
    QDateTime time = QDateTime::currentDateTime();
    QString nowtime = time.toString("yyyy-MM-dd hh:mm:ss");
    ui->textEdit_log->append(nowtime + "    Client:");
    ui->textEdit_log->append("    " + str);
    tcpSocket->write(ui->textEdit_input->toPlainText().toUtf8()); //toLatin1
//    saveMessage(nowtime, "Client", str);
}

// 接收Server发来的消息，并显示到消息记录框里。
void Client::receiveMessage()
{
    QDateTime time = QDateTime::currentDateTime();
    QString nowtime = time.toString("yyyy-MM-dd hh:mm:ss");
    QString str = tcpSocket->readAll();
    ui->textEdit_log->append(nowtime + "    Server:");
    ui->textEdit_log->append("    " + str);
}

// 将信息存储到数据库
//void Client::saveMessage(QString time, QString user, QString content)
//{
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
//          QString savesql = QString("INSERT INTO chat(time, user, content)");
//          savesql += QString(" VALUES('%1','%2','%3')").arg(time).arg(user).arg(content);
//     }
//}

//// 注册按钮，并存储到数据库里。
//void Client::on_logonbutton_clicked()
//{
//    QSqlDatabase dataBase=QSqlDatabase::addDatabase("QMYSQL");
//    dataBase.setHostName("localhost");
//    dataBase.setUserName("vici");
//    dataBase.setPassword("123456");
//    dataBase.setDatabaseName("ly");
//    dataBase.open();
//    QSqlQuery query(dataBase);
//    QString ID = ui->IDLineEdit->text();
//    QString PW = ui->PWLineEdit->text();
//    QString sql=QString("select *from user");
//    query.exec(sql);
//    if(query.numRowsAffected() == 0)
//    {
//          QString savesql = QString("INSERT INTO user(ID, PW)");
//          savesql += QString(" VALUES('%1','%2')").arg(ID).arg(PW);
//          bool ok=query.exec(savesql);
//          if(ok)
//          {
//               QMessageBox::about(NULL, "Save", "注册成功！");
//          }
//          else
//          {
//               QMessageBox::about(NULL, "Save", "注册失败！");
//          }
//     }
//}

void Client::displayError(QAbstractSocket::SocketError)
{
    qDebug() << tcpSocket->errorString();
}
