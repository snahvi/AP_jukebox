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

