#ifndef FORGOTPASSWORD_H
#define FORGOTPASSWORD_H

#include <QDialog>
#include <QMessageBox>
#include <QInputDialog>
#include "accountmanager.h"

namespace Ui {
class forgotpassword;
}

class forgotpassword : public QDialog
{
    Q_OBJECT

public:
    explicit forgotpassword(QWidget *parent = nullptr);
    ~forgotpassword();
    
    void setAccountManager(AccountManager *manager);

private slots:
    void on_buttonBox_accepted();
    void onPasswordRecoverySuccess();
    void onPasswordRecoveryFailed(const QString& reason);

private:
    Ui::forgotpassword *ui;
    AccountManager *accountManager;
};

#endif // FORGOTPASSWORD_H
