#include "client.h"
#include "loginform.h"
#include "registform.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LoginForm *loginWindow = new LoginForm();
    while (true)
    {
        if (loginWindow -> exec() == QDialog::Accepted)//调用login.exec()，阻塞主控制流，直到完成返回
        {
            Client w(nullptr,loginWindow->UserName);
            w.setWindowTitle("Client");
            w.show();
            return a.exec();
        }
        else
        {
            RegistrationForm *registrationWindow = new RegistrationForm();
            loginWindow -> hide();
            registrationWindow -> show();
            if (registrationWindow -> exec() == QDialog::Accepted)
            {
                loginWindow -> show();
            }
        }
    }
}
