#include "user.h"

User::User(const QString& username, const QString& passwordHash, const QString& securityAnswerHash)
    : username(username), passwordHash(passwordHash), securityAnswerHash(securityAnswerHash)
{
} 