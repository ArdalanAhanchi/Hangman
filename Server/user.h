#ifndef USER_H
#define USER_H

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class User {
public:
    // Constructors
    User();
    User(string username, string password);
    User(const User& copy);

    // getters
    string getUser();
    bool auth(string user, string pass);
    int getRoomId();
    void joined(int roomId);
    void left();

    string serialize();
    void deserialize(string serialized);

    void login();
    void logout();
    bool isLoggedIn();

private:
    string username;
    string password;
    int currRoomId;
    bool loginStat;
};

#endif /* USER_H */
