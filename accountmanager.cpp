#include "accountmanager.h"
#include <QFile>
#include <QTextStream>
#include <QCryptographicHash>
#include <QDebug>
#include <QByteArray>

AccountManager::AccountManager(QObject *parent) : QObject(parent) {
    loadUsers();
}

QString AccountManager::hashPassword(const QString& password) const {

    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return QString(hash.toHex());
}

bool AccountManager::registerUser(const QString& username, const QString& password, const QString& securityAnswer) {
    if (findUser(username)) {
        emit registrationFailed("Username already exists.");
        return false;
    }

    QString hashedPassword = hashPassword(password);
    QString hashedSecurityAnswer = hashPassword(securityAnswer);

    User* newUser = new User(username, hashedPassword, hashedSecurityAnswer);
    users.push_back(newUser);
    saveUsers();
    emit registrationSuccess(username);
    return true;
}

User* AccountManager::login(const QString& username, const QString& password) {
    User* user = findUser(username);
    if (!user) {
        emit loginFailed("Invalid username or password.");
        return nullptr;
    }

    if (user->getPasswordHash() == hashPassword(password)) {
        emit loginSuccess(user);
        return user;
    } else {
        emit loginFailed("Invalid username or password.");
        return nullptr;
    }
}

bool AccountManager::recoverPassword(const QString& username, const QString& securityAnswer, const QString& newPassword) {
    User* user = findUser(username);
    if (!user) {
        emit passwordRecoveryFailed("User not found.");
        return false;
    }

    if (user->getSecurityAnswerHash() == hashPassword(securityAnswer)) {
        user->setPasswordHash(hashPassword(newPassword));
        saveUsers();
        emit passwordRecoverySuccess();
        return true;
    } else {
        emit passwordRecoveryFailed("Incorrect security answer.");
        return false;
    }
}

void AccountManager::loadUsers() {
    QFile file(userDataFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open user data file for reading:" << file.errorString();
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split("::");
        if (parts.size() == 3) {
            User* user = new User(parts[0], parts[1], parts[2]);
            users.append(user);
        }
    }
    file.close();
}

void AccountManager::saveUsers() {
    QFile file(userDataFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        qWarning() << "Could not open user data file for writing:" << file.errorString();
        return;
    }

    QTextStream out(&file);
    for (User* user : users) {
        out << user->getUsername() << "::"
            << user->getPasswordHash() << "::"
            << user->getSecurityAnswerHash() << "\n";
    }
    file.close();
}

User* AccountManager::findUser(const QString& username) const {
    for (User* user : users) {
        if (user->getUsername() == username) {
            return user;
        }
    }
    return nullptr;
}
