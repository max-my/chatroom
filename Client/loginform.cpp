#include "loginform.h"
#include <QMessageBox>

LoginForm::LoginForm(QDialog *parent) :
    QDialog(parent)
{
    //设置窗体标题
    this -> setWindowTitle(tr("登录界面"));

    //用户名Label
    userNameLbl = new QLabel(this);
    userNameLbl -> move(70,80);//移动到(70,80)位置(Label左上角坐标，相对于父窗体)
    userNameLbl -> setText("用户名:");

    //用户名输入框
    userNameLEd = new QLineEdit(this);
    userNameLEd -> move(120,80);
    userNameLEd -> setPlaceholderText(tr("请输入用户名!"));//占位符

    //密码Label
    pwdLbl = new QLabel(this);
    pwdLbl -> move(80,130);
    pwdLbl -> setText("密码:");

    //密码输入框
    pwdLEd = new QLineEdit(this);
    pwdLEd -> move(120,130);
    pwdLEd -> setPlaceholderText("请输入密码!");
    pwdLEd -> setEchoMode(QLineEdit::Password);//输入的密码以圆点显示

    //注册按钮
    registraionBtn = new QPushButton(this);
    registraionBtn -> move(80,200);
    registraionBtn -> setText("注册");

    //登录按钮
    loginBtn = new QPushButton(this);
    loginBtn -> move(170,200);
    loginBtn -> setText("登录");

    //单击登录按钮时 执行 LoginForm::login 槽函数；//单击退出按钮时 执行 LoginForm::close 槽函数
    connect(loginBtn, &QPushButton::clicked, this, &LoginForm::login);
    connect(registraionBtn, &QPushButton::clicked, this, &LoginForm::registration);

}

void LoginForm::login()
{
    QFile *userNameFile = new QFile("userName.txt");
    QFile *pwdFile = new QFile("pwd.txt");
    userNameFile -> open(QIODevice::ReadOnly | QIODevice::Text);
    pwdFile -> open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream userNameOutPut(userNameFile);
    QTextStream pwdOutPut(pwdFile);
    while (! userNameOutPut.atEnd() && ! pwdOutPut.atEnd())
    {
        QString userName = userNameOutPut.readLine();
        QString pwd = pwdOutPut.readLine();
        //获得userNameLEd输入框的文本userNameLEd->text()；
        if (userName.compare(userNameLEd -> text()) == 0 && pwd.compare(pwdLEd -> text()) == 0)
        {
            UserName = userName;
            userNameFile -> close();
            pwdFile -> close();
            accept();//关闭窗体，并设置返回值为Accepted
            return;
        }
    }
    userNameFile -> close();
    pwdFile -> close();
    QMessageBox::warning(this, tr("警告！"), tr("用户名或密码错误！"), QMessageBox::Yes);
    // 清空内容
    userNameLEd -> clear();
    pwdLEd -> clear();
    //定位光标
    userNameLEd -> setFocus();
}

void LoginForm::registration()
{
    hide();
}
