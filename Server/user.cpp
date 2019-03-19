#include "user.h"

User::User()
{
    this->username = "";
    this->password = "";
    this->loginStat = false;
}

User::User(string username, string password)
{
    this->username = username;
    this->password = password;
    this->loginStat = false;
}

User::User(const User& copy)
{
    this->username = copy.username;
    this->password = copy.password;
    this->loginStat = false;
}

// getters
string User::getUser()
{
    return username;
}

bool User::auth(string user, string pass)
{
    return (username == user) && (password == pass);
}

int User::getRoomId()
{
    return currRoomId;
}

void User::joined(int roomId)
{
    currRoomId = roomId;
}

void User::left()
{
    currRoomId = 0;
}

void User::login()
{
    loginStat = true;
}

void User::logout()
{
    loginStat = false;
}

bool User::isLoggedIn()
{
    return loginStat;
}
