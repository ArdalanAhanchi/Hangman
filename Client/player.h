#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <iostream>

using namespace std;

class Player
{
public:
    //Constructors.
    Player();
    Player(string pName);

    //Getters.
    string getName();
    int getLevel();
    int getPercentage();

    //Setters.
    void setLevel(int newLevel);
    void setPercentage(int newPercentage);

    //For transmitting over network.

    //Returns a string representation of object.
    string serialize();

    //Constructs this object based on serialized.
    void deserialize(string serialized);

private:
    string name;            //Player name.
    int level;              //Current level.
    int percentage;         //Current percentage.
};

#endif // PLAYER_H
