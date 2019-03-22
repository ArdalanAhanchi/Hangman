#ifndef USER_H
#define USER_H

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>

#define USER_DELIMITER "*"

using namespace std;

class User {
public:
    // Constructors
    User();
    User(string username, string password);
    User(const User& copy);
    User(string serialized);

    // getters
    string getUser();
    bool auth(string user, string pass);
    int getRoomId();
    void joined(int roomId);
    void left();

    //For sending over network.
    string serialize();

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
