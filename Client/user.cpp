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

//Constructs this object based on serialized.
User::User(string serialized)
{
    string variables[2];                                                        //For holding strings for name, level, percentage.
    int currIndex = 0;                                                          //For knowing which var we're deserializing.
    for(size_t i = 0; i < serialized.size(); i++)
    {
        if(serialized[i] == USER_DELIMITER[0])                  //Go through every character, and seperate by :
            currIndex++;
        else
            variables[currIndex] = variables[currIndex] + (serialized[i]);
    }

    if(currIndex != 1)                                                          //Check for more than 2 seperators, or delimiter in text.
    {
        cerr << "User: Error, Serialization failed! invalid input." << endl;
        username = "";                                                          //Initialize all the variables.
        password = "";
        loginStat = false;
    }
    else
    {
        username = variables[0];                                                //Initialize all the variables.
        password = variables[1];
        loginStat = false;
    }
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

//Returns a string representation of object.
string User::serialize()
{
    string serialized = "";                             //Add the fields, serialize...
    serialized.append(username);                        //... by a : seperator.
    serialized.append(string(USER_DELIMITER));
    serialized.append(password);

    return serialized;
}
