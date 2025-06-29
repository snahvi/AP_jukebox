#include "signin.h"
#include "ui_signin.h"
#include "forgotpassword.h"

signin::signin(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::signin)
    , accountManager(nullptr)
{
    ui->setupUi(this);
    
    // Apply clean styling programmatically
    this->setStyleSheet(
        "QDialog { background-color: #f0f0f0; }"
        "QLabel { color: #333; font-weight: bold; }"
        "QLineEdit { background-color: white; color: black; border: 1px solid #ccc; border-radius: 5px; padding: 5px; font-size: 12px; }"
        "QPushButton { background-color: #28a745; color: white; border: none; border-radius: 5px; padding: 8px; }"
        "QPushButton#forgotpassword_btn { background-color: #007acc; color: white; border: none; border-radius: 5px; padding: 8px; }"
    );
    this->setWindowTitle("Sign In");
    
    // Connect the button programmatically
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &signin::on_buttonBox_accepted);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

signin::~signin()
{
    delete ui;
}

void signin::setAccountManager(AccountManager *manager)
{
    accountManager = manager;
    if (accountManager) {
        connect(accountManager, &AccountManager::loginSuccess, this, &signin::onLoginSuccess);
        connect(accountManager, &AccountManager::loginFailed, this, &signin::onLoginFailed);
    }
}

void signin::on_forgotpassword_btn_clicked()
{
    forgotpassword* p = new forgotpassword(this);
    if (accountManager) {
        p->setAccountManager(accountManager);
    }
    p->show();
}

void signin::on_buttonBox_accepted()
{
    if (!accountManager) {
        QMessageBox::warning(this, "Error", "Account manager not initialized.");
        return;
    }
    
    QString username = ui->username_le->text().trimmed();
    QString password = ui->password_le->text();
    
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter both username and password.");
        return;
    }
    
    accountManager->login(username, password);
}

void signin::onLoginSuccess(User* user)
{
    QMessageBox::information(this, "Success", "Login successful! Welcome, " + user->getUsername() + "!");
    accept();
    this->close();
}

void signin::onLoginFailed(const QString& reason)
{
    QMessageBox::warning(this, "Login Failed", reason);
}

