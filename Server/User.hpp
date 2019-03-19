#ifndef User_hpp
#define User_hpp

#include <stdlib.h>
#include <stdio.h>
using namespace std;

class User {
public:
    // Constructors
    User(string username, string password);
    
    // getters
    string getUser(string username);
    string getPubKey(string username);

    // setters
    void setUser(string userName);
private:
    string username;
    string password;
    string publicKey;
    int currRoomID;
}

#endif /* User_hpp */
