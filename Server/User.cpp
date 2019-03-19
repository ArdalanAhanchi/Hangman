#include "User.hpp"


User::User(string username, string password) {
    this.username = username;
    this.password = password;
//    this.publicKey = rand() % 10000; // how should this be generated and stored?
//    currRoomID = ?
}

// getters
string User::getUser(string username) {
    if (this.username.equals(username)) {
        return username;
    } else {
        return NULL;
    }
    
}

string User::getPubKey(string username) {
    if (this.username.equals(username)) {
        return publicKey;
    } else {
        return NULL;
    }
}

// setters
void User::setUser(string userName) {
    this.username = userName;
}
