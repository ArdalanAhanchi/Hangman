
#ifndef User_hpp
#define User_hpp

#include <stdio.h>
using namespace std;

class User {
public:
    // Constructors
    User(string username);
    
    // getters
    string getUser(string publicKey);
    string getPubKey(string username);

    // setters
    void setUser(string userName);
private:
    string username;
    string publicKey;
    int currRoomID;
}

#endif /* User_hpp */
