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
    Client client;
    // 调用nextPendingConnection获取连接进来的socket
    client.tcpSocket = tcpServer->nextPendingConnection();

    // 接受Client发来的消息，readyRead()准备读取信号，异步读取数据。
    connect(client.tcpSocket, SIGNAL(readyRead()), this, SLOT(receiveMessage()));

    // 把其他人昵称发送至新用户
    QString temp_names;
    for (int i=0; i<(int)(ClientArr.size()); i++)
    {
        temp_names += ClientArr[i].name + ":";
        if(i == (int)ClientArr.size() - 1)
        {
            client.tcpSocket->write(temp_names.toUtf8());
        }
    }
    // 把套接字加入vector
    ClientArr.push_back(client);
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
    if(temp_str=="")//禁止发空信息
        return;

    ui->textEdit_input->clear();
    ui->textEdit_input->setFocus();

    //获取当前时间
    QDateTime time = QDateTime::currentDateTime();
    QString nowtime = time.toString("yyyy-MM-dd hh:mm:ss");

    //显示在Server的消息记录里

    ui->textEdit_log->append("<b>" + nowtime + " <font color=red>GOD</font></b>");
    ui->textEdit_log->setAlignment(Qt::AlignRight);
    ui->textEdit_log->append(temp_str);

    QString str_msg = "GOD:" + temp_str;
    for(int j=0; j<(int)(ClientArr.size()); j++)
    {
        // 将消息发送到客户端
        ClientArr[j].tcpSocket->write(str_msg.toUtf8());
    }
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
    for(int i=0; i<(int)(ClientArr.size()); i++)
    {
        if(ClientArr[i].tcpSocket->bytesAvailable())
        {
            // 使用readAll函数读取所有信息
            QString temp_str = ClientArr[i].tcpSocket->readAll();

            if(ClientArr[i].name=="")// 第一次接收客户端信息一定是客户昵称
            {
                ui->textEdit_log->append("<b><font color=red>" + temp_str + "已上线</font></b>");
                ui->textEdit_log->setAlignment(Qt::AlignCenter);

                ClientArr[i].name = temp_str;
                update_member_list();

                // 把该用户昵称发给其他用户
                temp_str = temp_str + "*";
                for(int j=0; j<(int)(ClientArr.size()); j++)
                {
                    if(j != i)
                        ClientArr[j].tcpSocket->write(temp_str.toUtf8());
                }
                return;
            }

            if(temp_str.contains("@"))// 私聊请求处理//如何屏蔽发言中的特殊符号？
            {
                for(int j=0; j<(int)(ClientArr.size()); j++)
                {
                    if(ClientArr[j].name == temp_str.left(temp_str.indexOf("@")))//找到私聊对象
                        ClientArr[j].tcpSocket->write((ClientArr[i].name + "@" + temp_str.mid(temp_str.indexOf("@")+1)).toUtf8());//发送私聊信息
                }

                return;
            }

            if(temp_str.contains("%"))// 有人下线
            {
                QString PityGuy = temp_str.left(temp_str.indexOf("%"));

                ui->textEdit_log->append("<b><font color=red>" + PityGuy + "已下线</font></b>");
                ui->textEdit_log->setAlignment(Qt::AlignCenter);

                ClientArr.erase(ClientArr.begin()+i);// 不再维护该用户的连接

                for(int j=0; j<(int)(ClientArr.size()); j++)
                {
                    // 将消息发送到其他客户端
                    ClientArr[j].tcpSocket->write(temp_str.toUtf8());
                }
                update_member_list();
                return;
            }

            QDateTime time = QDateTime::currentDateTime();
            QString nowtime = time.toString("yyyy-MM-dd hh:mm:ss");

            int temp_pos = temp_str.indexOf(":");
            QString temp_name = temp_str.left(temp_pos);
            QString temp_msg = temp_str.mid(temp_pos+1);

            // 显示消息
            ui->textEdit_log->append("<b>" + nowtime + " <font color=blue>" + temp_name + "</font></b>");
            ui->textEdit_log->setAlignment(Qt::AlignLeft);
            ui->textEdit_log->append(temp_msg);

            for(int j=0; j<(int)(ClientArr.size()); j++)
            {
                // 将消息发送到其他客户端
                if(j != i)
                    ClientArr[j].tcpSocket->write(temp_str.toUtf8());
            }
        }
    }
}


// 异常信息
//void Server::displayError(QAbstractSocket::SocketError)
//{
//     qDebug() << tcpSocketConnection->errorString();
//}

// 快捷键
void Server::keyReleaseEvent(QKeyEvent *event)
{
    if(ui->textEdit_input->hasFocus())
    {
        if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        {
            sendMessage();
        }
    }
}

// 更新用户列表
void Server::update_member_list()
{
    ui->listWidget_MB->clear();
    for(int i=0; i<(int)ClientArr.size(); i++)
    {
        ui->listWidget_MB->addItem(ClientArr[i].name);
    }
}

// 踢人
void Server::on_pushButton_Fuckoff_clicked()
{
    // 获取对象昵称
    QString PityGuy = ui->listWidget_MB->currentItem()->text();
    QString temp_msg = "您确定让" + PityGuy + "滚蛋？";

    QMessageBox msgbox;
    msgbox.setModal(false);
    msgbox.setStandardButtons(QMessageBox::Yes);
    msgbox.setStandardButtons(QMessageBox::No);
    if(QMessageBox::Yes == QMessageBox::question(NULL,"注意",temp_msg))
    {
        for(int j=0; j<(int)(ClientArr.size()); j++)
        {
            // 将消息发送给所有人
            ClientArr[j].tcpSocket->write((PityGuy + "%").toUtf8());
            ClientArr[j].tcpSocket->waitForBytesWritten();
            if(ClientArr[j].name == PityGuy)
            {
                ClientArr.erase(ClientArr.begin()+j);// 不再维护该用户的连接
            }
        }

        ui->textEdit_log->append("<b><font color=red>" + PityGuy + "已被踢</font></b>");
        ui->textEdit_log->setAlignment(Qt::AlignCenter);
        update_member_list();
    }
}
