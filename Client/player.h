#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <iostream>

//  Ardalan Ahanchi
//  CSS 432
//  Hangman

using namespace std;

class Player
{
public:
    //Constructors.
    Player();
    Player(string pName);

    //Getters.
    string getName() const;
    int getLevel() const;
    int getPercentage() const;

    //Setters.
    void setLevel(int newLevel);
    void setPercentage(int newPercentage);

    //For transmitting over network.

    //Returns a string representation of object.
    string serialize();

    //Constructs this object based on serialized.
    void deserialize(string serialized);

    //Operator Overloads for soting.
    bool operator<(Player const &other);
    bool operator==(Player const &other);

private:
    string name;            //Player name.
    int level;              //Current level.
    int percentage;         //Current percentage.
};

#endif // PLAYER_H
