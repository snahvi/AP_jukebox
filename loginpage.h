#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QMainWindow>
#include "accountmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class LoginPage;
}
QT_END_NAMESPACE

class LoginPage : public QMainWindow
{
    Q_OBJECT

public:
    LoginPage(QWidget *parent = nullptr);
    ~LoginPage();

private slots:
    void on_signup_btn_clicked();

    void on_signin_btn_clicked();

private:
    Ui::LoginPage *ui;
    AccountManager *accountManager;
};
#endif // LOGINPAGE_H
