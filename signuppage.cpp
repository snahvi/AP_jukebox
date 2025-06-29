#include "signuppage.h"
#include "ui_signuppage.h"

signuppage::signuppage(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::signuppage)
    , accountManager(nullptr)
{
    ui->setupUi(this);
    
    // Connect the button programmatically
    connect(ui->cancel_btn, &QDialogButtonBox::accepted, this, &signuppage::on_cancel_btn_accepted);
    connect(ui->cancel_btn, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

signuppage::~signuppage()
{
    delete ui;
}

void signuppage::setAccountManager(AccountManager *manager)
{
    accountManager = manager;
    if (accountManager) {
        connect(accountManager, &AccountManager::registrationSuccess, this, &signuppage::onRegistrationSuccess);
        connect(accountManager, &AccountManager::registrationFailed, this, &signuppage::onRegistrationFailed);
    }
}

void signuppage::on_cancel_btn_accepted()
{
    if (!accountManager) {
        QMessageBox::warning(this, "Error", "Account manager not initialized.");
        return;
    }
    
    QString username = ui->lineEdit_4->text().trimmed();
    QString password = ui->lineEdit_10->text();
    QString securityAnswer = ui->lineEdit_9->text().trimmed();
    
    if (username.isEmpty() || password.isEmpty() || securityAnswer.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all fields.");
        return;
    }
    
    if (password.length() < 6) {
        QMessageBox::warning(this, "Error", "Password must be at least 6 characters long.");
        return;
    }
    
    accountManager->registerUser(username, password, securityAnswer);
}

void signuppage::onRegistrationSuccess(const QString& username)
{
    QMessageBox::information(this, "Success", "Registration successful! Welcome, " + username + "!");
    accept();
}

void signuppage::onRegistrationFailed(const QString& reason)
{
    QMessageBox::warning(this, "Registration Failed", reason);
}

