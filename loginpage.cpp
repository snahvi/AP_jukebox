#include "loginpage.h"
#include "./ui_loginpage.h"
#include "signuppage.h"
#include "signin.h"


LoginPage::LoginPage(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LoginPage)
    , accountManager(new AccountManager(this))
{
    ui->setupUi(this);
    
    // Ensure Batman background is applied
    this->setStyleSheet(
        "LoginPage {"
        "background-image: url(C:/Users/LOQ/Downloads/batman1.jpg);"
        "background-repeat: no-repeat;"
        "background-position: center;"
        "background-attachment: fixed;"
        "background-size: cover;"
        "}"
        "QWidget#centralwidget { background: transparent; }"
        "QLabel#Home_page_welcome {"
        "background-color: rgba(0, 0, 0, 150);"
        "color: white;"
        "border-radius: 10px;"
        "padding: 10px;"
        "}"
        "QPushButton {"
        "background-color: rgba(30, 30, 30, 200);"
        "color: white;"
        "border: 2px solid #FFD700;"
        "border-radius: 15px;"
        "padding: 10px;"
        "}"
    );
}

LoginPage::~LoginPage()
{
    delete ui;
}

void LoginPage::on_signup_btn_clicked()
{
    signuppage *p = new signuppage(this);
    p->setAccountManager(accountManager);
    p->show();
}


void LoginPage::on_signin_btn_clicked()
{
    signin *p = new signin(this);
    p->setAccountManager(accountManager);
    p->show();
}

