#include "User.hpp"


User::User(string username) {
    this.username = username;
//    this.publicKey = rand() % 10000; // how should this be generated and stored?
//    currRoomID = ?
}

// getters
string User::getUser(string publicKey) {
    if (this.publicKey.equals(publicKey)) {
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
