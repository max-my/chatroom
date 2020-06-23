#include "client.h"
#include "ui_client.h"

Client::Client(QWidget *parent, QString user_name)
    : QMainWindow(parent)
    , ui(new Ui::Client)
    , is_connect(false)
    , get_names(false)
    , is_Pai_Others(false)
    , send_Pai_to_others(false)
{
    ui->setupUi(this);

    ui->lineEdit_name->setPlaceholderText(user_name);

    // 关联连接按钮和函数，连接到服务器
    connect(ui->pbConnect,SIGNAL(clicked()),this,SLOT(on_connectButton_clicked()));

    // 将发送按钮和sendMessage函数关联起来
    connect(ui->pbSend,SIGNAL(clicked(bool)),this,SLOT(sendMessage()));
    connect(ui->pbSend2,SIGNAL(clicked(bool)),this,SLOT(sendPrivateMessage()));
}


Client::~Client()
{
    offline();// 通知服务器我下线了
    delete ui;
}

void Client::displayError(QAbstractSocket::SocketError)
{
    qDebug() << tcpSocket->errorString();
}

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
            QMessageBox::critical(NULL, "错误", "请输入昵称");
            return;
        }
        else if(str_name.contains(":") || str_name.contains("@") || str_name.contains("*") || str_name.contains("^") || str_name.contains("%") || str_name.contains("#") || str_name.contains("&") || str_name.contains("$"))
        {
            QMessageBox::critical(NULL, "错误", "昵称中不允许包含\":^@*%#&$\"！");
            return;
        }
        else if(str_name=="GOD")
        {
            QMessageBox::critical(NULL, "放肆", "这里只能有一个神！");
            return;
        }


        if(!is_name_exist)// 第一次发送
        {
            tcpSocket = new QTcpSocket(this);
            connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError(QAbstractSocket::SocketError)));
            // 接受Server发来的消息，readyRead()准备读取信号，异步读取数据。
            connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(receiveMessage()));

            // 连接服务器
            tcpSocket->connectToHost("127.0.0.1", 7777);
            if(tcpSocket->waitForConnected(10000))
            {
                is_connect = true;
                ui->pbConnect->setText("退出");
                ui->textEdit_notice->append("连接成功！");
                // 第一次连接发送客户昵称
                tcpSocket->write((str_name + "^").toUtf8());
            }
            else
            {
                QMessageBox::about(NULL,"Connection","连接失败，请检查服务器是否开启！");
            }
        }
        else //修改昵称
        {
            // 发送客户昵称
            tcpSocket->write((str_name + "^").toUtf8());
        }
    }
    else
    {
        offline();// 下线
    }
}

// 下线
void Client::offline()
{
    // 发送下线信息
    tcpSocket->write((str_name + "%").toUtf8());

    // 清空套接字
    tcpSocket->disconnectFromHost();
    tcpSocket->waitForDisconnected();
    delete tcpSocket;//不能delete，因为初始化在构造函数里//挪了初始化位置

    // 初始化全局变量
    str_name = "";
    str_friend = "";
    str_names.clear();
    update_member_list();

    is_Pai_Others = false;
    send_Pai_to_others = false;
    is_connect = false;
    get_names = false;

    ui->pbConnect->setText("连接");
    ui->textEdit_notice->append("断开连接！");
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
    int temp_pos = temp_str.indexOf(":");
    QString temp_name = temp_str.left(temp_pos);
    QString temp_msg = temp_str.mid(temp_pos+1);

    if (temp_str.contains("$"))// 重新改名字
    {

        QMessageBox::critical(NULL, "注意", "昵称重复！ 重新修改昵称！");
        ui->pbConnect->setText("修改");
        is_connect=false;
        is_name_exist = true;
        return;
    }
    if (temp_str.contains("&"))
    {
        ui->textEdit_notice->append("登录成功！");
        ui->pbConnect->setText("退出");
        is_connect=true;
        is_name_exist = false;
        return;
    }

    if(is_connect)
    {
        if(temp_name == "GOD")//服务器发来的消息在notice显示
        {
            ui->textEdit_notice->append("<b><font color=red>" + temp_str + "</font></b>");
            return;
        }
        else if (temp_str.contains("*"))// 添加用户
        {
            // 更新成员列表
            temp_str = temp_str.left(temp_str.indexOf("*"));
            str_names.push_back(temp_str);
            update_member_list();

            ui->textEdit_notice->append("<font color=blue><b>" + temp_str + "</b>开着跑车进入聊天室,快去拍Ta！</font>");

            if(!get_names)//处理第一个用户特殊情况
                get_names = true;

            return;
        }
        else if (temp_str.contains("%"))// 删除用户
        {
            // 更新成员列表
            temp_str = temp_str.left(temp_str.indexOf("%"));
            if(temp_str == str_name)//我被踢了
            {
                fuck_GOD();
                return;
            }
            for(int i=0; i<(int)str_names.size(); i++)// 删除下线的成员
            {
                if(str_names[i] == temp_str)
                {
                    str_names.erase(str_names.begin()+i);
                    break;
                }
            }
            update_member_list();
            ui->textEdit_notice->append("<font color=red><b>" + temp_str + "</b>坐着火箭离开聊天室！！！！</font>");
            return;
        }
        else if (temp_str.contains("@"))// 切换私聊
        {
            // 询问接不接受私聊
            handle_private_chat(temp_str);
            return;
        }
        else if(!get_names)// 第一次接收，存储已登录的用户信息// 位置必须在添加用户后！
        {
            int temp_num = temp_str.count(":");
            for (int i=0; i<temp_num; i++)
            {
                // 取出一个
                int temp_pos = temp_str.indexOf(":");
                QString temp_name = temp_str.left(temp_pos);
                // 压入一个
                str_names.push_back(temp_name);
                // 截断
                temp_str = temp_str.mid(temp_pos+1);
            }
            update_member_list();
            get_names = true;
            return;
        }

        // 正常消息显示
        QDateTime time = QDateTime::currentDateTime();
        QString nowtime = time.toString("yyyy-MM-dd hh:mm:ss");

        ui->textEdit_log->append("<b>" + nowtime + " <font color=blue>" + temp_name + "</font></b>");
        ui->textEdit_log->setAlignment(Qt::AlignLeft);
        ui->textEdit_log->append(temp_msg);
    }
}

// 被踢了
void Client::fuck_GOD()
{
    QMessageBox msgbox;
    msgbox.setModal(false);
    msgbox.setStandardButtons(QMessageBox::Yes);
    msgbox.button(QMessageBox::Yes)->setText("Damn it");
    QMessageBox::information(NULL, "GOD", "Fuck off");

    // 清空套接字
    tcpSocket->disconnectFromHost();
    tcpSocket->waitForDisconnected();
    delete tcpSocket;

    // 初始化全局变量
    str_name = "";
    str_friend = "";
    str_names.clear();
    update_member_list();

    is_Pai_Others = false;
    send_Pai_to_others = false;
    is_connect = false;
    get_names = false;

    ui->pbConnect->setText("连接");
    ui->textEdit_notice->append("断开连接！");
}

// 私聊按钮
void Client::on_pbPaita_clicked()
{
    // 获取对象昵称
    str_friend = ui->listWidget_MB->currentItem()->text();

    QMessageBox msgbox;
    msgbox.setModal(false);// 设置非模态
    QMessageBox::information(NULL, "提示", "请耐心等待"+str_friend+"同意");

    // 切换到私聊页面
    ui->label_Name->setText("<b>"+str_friend+"的小窗</b>");

    ui->label_Name->setAlignment(Qt::AlignCenter);
    QFont font;
    font.setPointSize(20); // 设置字号
    ui->label_Name->setFont(font);  // 设置字体

    ui->textEdit_log2->clear();
    ui->textEdit_log2->append("<b>" + str_friend + "正在赶来...</b>");

    // 发送私聊请求
    tcpSocket->write((str_friend+"@").toUtf8());
    send_Pai_to_others = true;
    ui->stackedWidget->setCurrentIndex(1);
}

// 返回大厅
void Client::on_pbReturn_clicked()
{
    // 返回大厅页面
    // 提示对方自己离开
    if(is_Pai_Others)
    {
        if(QMessageBox::Yes == QMessageBox::question(this,QStringLiteral("注意"),QStringLiteral("返回大厅将断开私聊连接！"),QMessageBox::Yes | QMessageBox:: No))
        {
            //        qDebug()<<"yes";
            tcpSocket->write((str_friend+"@#").toUtf8());
            str_friend = "";
            send_Pai_to_others = false;
            is_Pai_Others = false;
            // 返回大厅
            ui->stackedWidget->setCurrentIndex(0);
        }
        else
        {
            //        qDebug()<<"no";
            return;
        }
    }
    else
    {
        // 返回大厅
        ui->stackedWidget->setCurrentIndex(0);
    }
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
    if(ui->textEdit_input2->hasFocus())
    {
        if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        {
            sendPrivateMessage();
        }
    }
}

// 更新用户列表
void Client::update_member_list()
{
    ui->listWidget_MB->clear();
    for(int i=0; i<(int)str_names.size(); i++)
    {
        ui->listWidget_MB->addItem(str_names[i]);
    }
}

// 处理私聊
void Client::handle_private_chat(QString str_msg)
{
    if(send_Pai_to_others && !is_Pai_Others)//我拍了别人
    {
        if(str_friend == str_msg.left(str_msg.indexOf("@")))//是我拍的人发来的
        {
            if("YES" == str_msg.mid(str_msg.indexOf("@")+1))//同意
            {
                ui->textEdit_log2->append("<b>" + str_friend + "大摇大摆的进来了</b>");
                is_Pai_Others = true;
            }
            else//fuck
            {
                ui->textEdit_log2->append("<b>" + str_friend + "拒绝了你的邀请..bad bad</b>");
                send_Pai_to_others = false;
            }
        }
        else// 其他人发来的，直接拒绝，做一个专一的男人
        {
            tcpSocket->write((str_msg+"NO").toUtf8());
        }
        return;
    }
    else if(is_Pai_Others)// 正在互拍
    {
        int temp_pos = str_msg.indexOf("@");
        QString temp_name = str_msg.left(temp_pos);
        QString temp_msg = str_msg.mid(temp_pos+1);

        if(temp_msg == "#")//对方离开了
        {
            ui->textEdit_log2->append("<b>" + str_friend + "已离开...</b>");
            ui->textEdit_log2->setAlignment(Qt::AlignLeft);
            str_friend = "";
            send_Pai_to_others = false;
            is_Pai_Others = false;
            return;
        }

        QDateTime time = QDateTime::currentDateTime();
        QString nowtime = time.toString("yyyy-MM-dd hh:mm:ss");

        ui->textEdit_log2->append("<b>" + nowtime + " <font color=blue>" + temp_name + "</font></b>");
        ui->textEdit_log2->setAlignment(Qt::AlignLeft);
        ui->textEdit_log2->append(temp_msg);
    }
    else // 没有在拍，也没有拍人
    {
        QString temp_msg = str_msg.left(str_msg.indexOf("@")) + "想拍你";

        QMessageBox msgbox;
        msgbox.setModal(false);// 设置非模态
        if(QMessageBox::Yes == QMessageBox::question(NULL,"注意",temp_msg,QMessageBox::Yes | QMessageBox:: No))
        {
            //        qDebug()<<"yes";
            str_friend = str_msg.left(str_msg.indexOf("@"));
            tcpSocket->write((str_friend+"@YES").toUtf8());

            // 切换到私聊页面
            ui->label_Name->setText("<b>"+str_friend+"的小窗</b>");

            ui->label_Name->setAlignment(Qt::AlignCenter);
            QFont font;
            font.setPointSize(20); // 设置字号
            ui->label_Name->setFont(font);  // 设置字体

            ui->textEdit_log2->clear();
            ui->textEdit_log2->append("<b>快和" + str_friend + "打个招呼吧!</b>");

            is_Pai_Others = true;
            ui->stackedWidget->setCurrentIndex(1);
        }
        else
        {
            //        qDebug()<<"no";
            tcpSocket->write((str_msg+"NO").toUtf8());
            return;
        }
    }
}

// 发送私聊信息
void Client::sendPrivateMessage()
{
    if(!is_Pai_Others)//没人给拍不能发消息
    {
        if(send_Pai_to_others)
        {
            QMessageBox msgbox;
            msgbox.setModal(false);// 设置非模态
            QMessageBox::information(NULL,"注意","请耐心等待");
        }
        else
        {
            QMessageBox msgbox;
            msgbox.setModal(false);// 设置非模态
            QMessageBox::information(NULL,"注意","请返回大厅重新拍人");
            ui->textEdit_input2->clear();
            ui->pbReturn->setFocus();
        }
        return;
    }
    QString temp_str = ui->textEdit_input2->toPlainText();
    if(temp_str=="")
        return;

    ui->textEdit_input2->clear();
    ui->textEdit_input2->setFocus();

    QDateTime time = QDateTime::currentDateTime();
    QString nowtime = time.toString("yyyy-MM-dd hh:mm:ss");


    ui->textEdit_log2->append("<b>" + nowtime + " <font color=red>" + str_name + "</font></b>");
    ui->textEdit_log2->setAlignment(Qt::AlignRight);// 奇怕！这一行会控制上下两行的对齐方式
    ui->textEdit_log2->append(temp_str);

    QString str_msg = str_friend + "@" + temp_str;//中间的标志变为了@
    tcpSocket->write(str_msg.toUtf8()); //toLatin1
    //    saveMessage(nowtime, "Client", str);
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
