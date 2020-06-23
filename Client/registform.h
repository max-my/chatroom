#ifndef REGISTFORM_H
#define REGISTFORM_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFile>
#include <QIODevice>
#include <QTextStream>

class RegistrationForm : public QDialog
{
    Q_OBJECT

public:
    explicit RegistrationForm(QDialog *parent = 0);

public slots:
    void registration();
    void returnLogin();
    void closeEvent(QCloseEvent *event);

private:
    QLabel *userNameLbl;
    QLabel *pwdLbl;
    QLabel *identifyLbl;
    QLineEdit *userNameLEd;
    QLineEdit *pwdLEd;
    QLineEdit *identifyLEd;
    QPushButton *registrationBtn;
    QPushButton *returnBtn;
};

#endif // REGISTFORM_H
