#include "registform.h"
#include <QMessageBox>

RegistrationForm::RegistrationForm(QDialog *parent) :
    QDialog(parent)
{
    //设置窗体标题
    this -> setWindowTitle(tr("注册界面"));

    //用户名Label
    userNameLbl = new QLabel(this);
    userNameLbl -> move(50,80);//移动到(70,80)位置(Label左上角坐标，相对于父窗体)
    userNameLbl -> setText("用户名:");

    //用户名输入框
    userNameLEd = new QLineEdit(this);
    userNameLEd -> move(120,80);
    userNameLEd -> setPlaceholderText(tr("请输入用户名!"));//占位符

    //密码Label
    pwdLbl = new QLabel(this);
    pwdLbl -> move(50,130);
    pwdLbl -> setText("密码:");

    //密码输入框
    pwdLEd = new QLineEdit(this);
    pwdLEd -> move(120,130);
    pwdLEd -> setPlaceholderText("请输入密码!");
    pwdLEd -> setEchoMode(QLineEdit::Password);//输入的密码以圆点显示

    //确认密码Label
    identifyLbl = new QLabel(this);
    identifyLbl -> move(50,180);
    identifyLbl -> setText("确认密码:");

    //确认密码输入框
    identifyLEd = new QLineEdit(this);
    identifyLEd -> move(120,180);
    identifyLEd -> setPlaceholderText("请确认密码!");
    identifyLEd -> setEchoMode(QLineEdit::Password);//输入的密码以圆点显示

    //注册按钮
    registrationBtn = new QPushButton(this);
    registrationBtn -> move(80,230);
    registrationBtn -> setText("注册");

    //返回按钮
    returnBtn = new QPushButton(this);
    returnBtn -> move(170,230);
    returnBtn -> setText("返回");

    //单击注册按钮时 执行 RegistrationForm::registration 槽函数
    connect(registrationBtn, &QPushButton::clicked, this, &RegistrationForm::registration);
    //单击返回按钮时 执行 RegistrationForm::returnLogin 槽函数
    connect(returnBtn, &QPushButton::clicked, this, &RegistrationForm::returnLogin);

}

void RegistrationForm::registration()
{
    if (pwdLEd -> text() == identifyLEd -> text())
    {
        QFile *userNameFile = new QFile("userName.txt");
        QFile *pwdFile = new QFile("pwd.txt");

        //保存userNameLEd输入框的文本userNameLEd -> text()；
        userNameFile -> open(QIODevice::ReadWrite | QIODevice::Append);
        QTextStream userNameOutPut(userNameFile);
        userNameOutPut << userNameLEd -> text() << endl;
        userNameFile -> close();

        //保存pwdLEd输入框的文本pwdLEd -> text()；
        pwdFile -> open(QIODevice::ReadWrite | QIODevice::Append);
        QTextStream pwdOutPut(pwdFile);
        pwdOutPut << pwdLEd -> text() << endl;
        pwdFile -> close();

        QMessageBox::information(this, tr("完成！"), tr("注册完成！"));
        // 清空内容
        userNameLEd -> clear();
        pwdLEd -> clear();
        identifyLEd -> clear();
        //定位光标
        userNameLEd -> setFocus();
        accept();//关闭窗体，并设置返回值为Accepted
    }
    else
    {
       QMessageBox::warning(this, tr("警告！"), tr("两次输入密码不一致！"), QMessageBox::Yes);
       // 清空内容
       userNameLEd -> clear();
       pwdLEd -> clear();
       identifyLEd -> clear();
       //定位光标
       userNameLEd -> setFocus();
    }
}

void RegistrationForm::returnLogin()
{
    reject();//关闭窗体，并设置返回值为Accepted
}

void RegistrationForm::closeEvent(QCloseEvent *event)
{
    done(10);
}
