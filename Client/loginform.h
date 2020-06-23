#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFile>
#include <QIODevice>
#include <QTextStream>

class LoginForm : public QDialog
{
    Q_OBJECT

public:
    explicit LoginForm(QDialog *parent = 0);
//    ~LoginForm();

    QString UserName;
public slots:
    void registration();
    void login();
    void closeEvent(QCloseEvent *event);

private:
    QLabel *userNameLbl;
    QLabel *pwdLbl;
    QLineEdit *userNameLEd;
    QLineEdit *pwdLEd;
    QPushButton *loginBtn;
    QPushButton *registraionBtn;
};

#endif // LOGINFORM_H
