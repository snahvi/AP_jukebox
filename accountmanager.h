#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include "user.h"
#include <QVector>
#include <QObject>

class AccountManager : public QObject {
    Q_OBJECT
public:
    explicit AccountManager(QObject *parent = nullptr);

    bool registerUser(const QString& username, const QString& password, const QString& securityAnswer);
    User* login(const QString& username, const QString& password);
    bool recoverPassword(const QString& username, const QString& securityAnswer, const QString& newPassword);

signals:
    void registrationSuccess(const QString& username);
    void registrationFailed(const QString& reason);
    void loginSuccess(User* user);
    void loginFailed(const QString& reason);
    void passwordRecoverySuccess();
    void passwordRecoveryFailed(const QString& reason);

private:
    QVector<User*> users;
    const QString userDataFile = "users.txt";

    QString hashPassword(const QString& password) const;

    void loadUsers();
    void saveUsers();
    User* findUser(const QString& username) const;
};

#endif
