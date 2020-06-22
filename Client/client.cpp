#include "client.h"
#include "ui_client.h"

Client::Client(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Client)
    , is_connect(false)
{
    ui->setupUi(this);

    ui->lineEdit_name->setPlaceholderText("请输入昵称");

    // ================通信相关=============== //
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
    if(!is_connect)
    {
        // 获取昵称
        str_name = ui->lineEdit_name->text();
        if(str_name.length()>10)
        {
            QMessageBox::critical(NULL, "错误", "昵称最多10个字符！");
            return;
        }
        else if(str_name.length()==0)
        {
            str_name = "游客";
            QMessageBox::warning(NULL, "警告", "你将以游客的身份进入大厅！");
        }
        else if(str_name.contains(":"))
        {
            QMessageBox::critical(NULL, "错误", "昵称中不允许包含\":\"！");
            return;
        }
        else if(str_name=="GOD")
        {
            QMessageBox::critical(NULL, "放肆", "这里只能有一个神！");
            return;
        }

        // 连接服务器
        tcpSocket->connectToHost("127.0.0.1", 7777);
        if(tcpSocket->waitForConnected(10000))
        {
            is_connect = true;
            ui->pbConnect->setText("退出");

            // 第一次连接发送客户昵称
            tcpSocket->write(str_name.toUtf8());

            ui->textEdit_notice->append("登录成功！");
        }
        else
        {
            QMessageBox::about(NULL,"Connection","登录失败！");
        }
    }
    else
    {
        // TODO:或许需要添加向服务器发送断开指示
        exit(0); // 会使所有窗口关闭，包括服务器，待修改！
    }
}

// 发送信息
void Client::sendMessage()
{
    if(!is_connect)
    {
        QMessageBox::critical(NULL, "错误", "请先连接服务器");
        return;
    }
    else
    {
        QString temp_str = ui->textEdit_input->toPlainText();
        if(temp_str=="")
            return;

        ui->textEdit_input->clear();
        ui->textEdit_input->setFocus();

        QDateTime time = QDateTime::currentDateTime();
        QString nowtime = time.toString("yyyy-MM-dd hh:mm:ss");


        ui->textEdit_log->append("<b>" + nowtime + " <font color=red>" + str_name + "</font></b>");
        ui->textEdit_log->setAlignment(Qt::AlignRight);// 奇怕！这一行会控制上下两行的对齐方式
        ui->textEdit_log->append(temp_str);

        QString str_msg = str_name + ":" + temp_str;
        tcpSocket->write(str_msg.toUtf8()); //toLatin1
        //    saveMessage(nowtime, "Client", str);
    }
}

// 接收Server发来的消息，并显示到消息记录框里。
void Client::receiveMessage()
{
    QString temp_str = tcpSocket->readAll();
    QDateTime time = QDateTime::currentDateTime();
    QString nowtime = time.toString("yyyy-MM-dd hh:mm:ss");
    int temp_pos = temp_str.indexOf(":");
    QString temp_name = temp_str.left(temp_pos);
    QString temp_msg = temp_str.mid(temp_pos+1);

    ui->textEdit_log->append("<b>" + nowtime + " <font color=blue>" + temp_name + "</font></b>");
    ui->textEdit_log->setAlignment(Qt::AlignLeft);
    ui->textEdit_log->append(temp_msg);
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

void Client::on_pbPaita_clicked()
{
    // 切换到私聊页面
    ui->stackedWidget->setCurrentIndex(1);
}

void Client::on_pbReturn_clicked()
{
    // 返回大厅页面
    ui->stackedWidget->setCurrentIndex(0);
}


// 快捷键
void Client::keyReleaseEvent(QKeyEvent *event)
{
    if(ui->textEdit_input->hasFocus())
    {
        if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        {
            sendMessage();
        }
    }
}
