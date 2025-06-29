#include "forgotpassword.h"
#include "ui_forgotpassword.h"

forgotpassword::forgotpassword(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::forgotpassword)
    , accountManager(nullptr)
{
    ui->setupUi(this);
    
    // Apply clean styling programmatically
    this->setStyleSheet(
        "QDialog { background-color: #f0f0f0; }"
        "QLabel { color: #333; font-weight: bold; }"
        "QLineEdit { background-color: white; color: black; border: 1px solid #ccc; border-radius: 5px; padding: 5px; font-size: 12px; }"
        "QPushButton { background-color: #28a745; color: white; border: none; border-radius: 5px; padding: 8px; }"
    );
    this->setWindowTitle("Forgot Password");
    
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
    this->close();
}

void forgotpassword::onPasswordRecoveryFailed(const QString& reason)
{
    QMessageBox::warning(this, "Password Recovery Failed", reason);
}
