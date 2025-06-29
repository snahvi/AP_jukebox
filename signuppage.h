#ifndef SIGNUPPAGE_H
#define SIGNUPPAGE_H

#include <QDialog>
#include <QMessageBox>
#include "accountmanager.h"

namespace Ui {
class signuppage;
}

class signuppage : public QDialog
{
    Q_OBJECT

public:
    explicit signuppage(QWidget *parent = nullptr);
    ~signuppage();
    
    void setAccountManager(AccountManager *manager);

private slots:
    void on_cancel_btn_accepted();
    void onRegistrationSuccess(const QString& username);
    void onRegistrationFailed(const QString& reason);

private:
    Ui::signuppage *ui;
    AccountManager *accountManager;
};

#endif // SIGNUPPAGE_H
