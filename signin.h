#ifndef SIGNIN_H
#define SIGNIN_H

#include <QDialog>
#include <QMessageBox>
#include "accountmanager.h"

namespace Ui {
class signin;
}

class signin : public QDialog
{
    Q_OBJECT

public:
    explicit signin(QWidget *parent = nullptr);
    ~signin();
    
    void setAccountManager(AccountManager *manager);

private slots:
    void on_forgotpassword_btn_clicked();
    void on_buttonBox_accepted();
    void onLoginSuccess(User* user);
    void onLoginFailed(const QString& reason);

private:
    Ui::signin *ui;
    AccountManager *accountManager;
};

#endif // SIGNIN_H
