#ifndef USER_H
#define USER_H

#include <QString>

class User {
public:
    User(const QString& username, const QString& passwordHash, const QString& securityAnswerHash);


    QString getUsername() const { return username; }
    QString getPasswordHash() const { return passwordHash; }
    QString getSecurityAnswerHash() const { return securityAnswerHash; }

    void setPasswordHash(const QString& newHash) { passwordHash = newHash; }

private:
    QString username;
    QString passwordHash;
    QString securityAnswerHash;
};

#endif
