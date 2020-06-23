#include "client.h"
#include "loginform.h"
#include "registform.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LoginForm *loginWindow = new LoginForm();
//    if (loginWindow -> exec() == QDialog::Accepted)//调用login.exec()，阻塞主控制流，直到完成返回
//    {
//        Client w(nullptr,loginWindow->UserName);
//        w.setWindowTitle("Client");
//        w.show();
//        return a.exec();
//    }
//    else
//    {
//        loginWindow->show();
//        return loginWindow->exec();
//    }


    while (true)
    {
        int f = loginWindow -> exec();
        if (f == QDialog::Accepted)//调用login.exec()，阻塞主控制流，直到完成返回
        {
            Client w(nullptr,loginWindow->UserName);
            w.setWindowTitle("Client");
            w.show();
            return a.exec();
        }
        else if(f == 10)
        {
            return 0;
        }
        else
        {

            RegistrationForm *registrationWindow = new RegistrationForm();
            loginWindow -> hide();
            registrationWindow -> show();
            int ff = registrationWindow -> exec();
            if (ff == QDialog::Accepted)
            {
                loginWindow -> show();
            }
//            else if(ff == 10)
//            {
//                return 0;
//            }
        }
    }
}
