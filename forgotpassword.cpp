#include "forgotpassword.h"
#include "ui_forgotpassword.h"

forgotpassword::forgotpassword(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::forgotpassword)
    , accountManager(nullptr)
{
    ui->setupUi(this);
    
    // Connect the button programmatically
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &forgotpassword::on_buttonBox_accepted);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

forgotpassword::~forgotpassword()
{
    delete ui;
}

void forgotpassword::setAccountManager(AccountManager *manager)
{
    accountManager = manager;
    if (accountManager) {
        connect(accountManager, &AccountManager::passwordRecoverySuccess, this, &forgotpassword::onPasswordRecoverySuccess);
        connect(accountManager, &AccountManager::passwordRecoveryFailed, this, &forgotpassword::onPasswordRecoveryFailed);
    }
}

void forgotpassword::on_buttonBox_accepted()
{
    if (!accountManager) {
        QMessageBox::warning(this, "Error", "Account manager not initialized.");
        return;
    }
    
    QString username = ui->username_le->text().trimmed();
    QString securityAnswer = ui->favouritecity_le->text().trimmed();
    
    if (username.isEmpty() || securityAnswer.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter both username and security answer.");
        return;
    }
    
    bool ok;
    QString newPassword = QInputDialog::getText(this, "New Password", 
                                               "Enter your new password:", 
                                               QLineEdit::Password, 
                                               "", &ok);
    
    if (ok && !newPassword.isEmpty()) {
        if (newPassword.length() < 6) {
            QMessageBox::warning(this, "Error", "Password must be at least 6 characters long.");
            return;
        }
        accountManager->recoverPassword(username, securityAnswer, newPassword);
    }
}

void forgotpassword::onPasswordRecoverySuccess()
{
    QMessageBox::information(this, "Success", "Password has been successfully updated!");
    accept();
}

void forgotpassword::onPasswordRecoveryFailed(const QString& reason)
{
    QMessageBox::warning(this, "Password Recovery Failed", reason);
}
