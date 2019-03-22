#include "player.h"

//  Ardalan Ahanchi
//  CSS 432
//  Hangman

//Default constructor.
Player::Player()
{
    name = "";
    level = 0;
    percentage = 0;
}

//Constructor with player name.
Player::Player(string pName)
{
    name = pName;
    level = 0;
    percentage = 0;
}

//Getters.
string Player::getName() const { return name; }
int Player::getLevel() const { return level; }
int Player::getPercentage() const { return percentage; }

//Setters.
void Player::setLevel(int newLevel)
{
    if(newLevel < 0)                                    //Check negative, return if negative.
    {
        cerr << "ERROR: Player level invalid." << endl;
        return;
    }

    level = newLevel;
}

void Player::setPercentage(int newPercentage)
{
    if(newPercentage < 0 || newPercentage > 100)        //Check range, return if invalid.
    {
        cerr << "ERROR: Player percentage invalid." << endl;
        return;
    }

    percentage = newPercentage;
}

//Returns a string representation of object.
string Player::serialize()
{
    string serialized = "";                             //Add the fields, serialize...
    serialized.append(name);                            //... by a : seperator.
    serialized.append(":");
    serialized.append(to_string(level));
    serialized.append(":");
    serialized.append(to_string(percentage));

    return serialized;
}

//Constructs this object based on serialized.
void Player::deserialize(string serialized)
{
    string variables[3];                                        //For holding strings for name, level, percentage.
    int currIndex = 0;                                          //For knowing which var we're deserializing.
    for(size_t i = 0; i < serialized.size(); i++)
    {
        if(serialized[i] == ':')                                //Go through every character, and seperate by :
            currIndex++;
        else
            variables[currIndex] = variables[currIndex] + (serialized[i]);
    }

    if(currIndex > 2)                                           //Check for more than 2 seperators.
    {
        cerr << "ERROR: Serialization failed! invalid input." << endl;
        return;
    }

    name = variables[0];                                        //Initialize all the variables.
    this->setLevel(stoi(variables[1]));
    this->setPercentage(stoi(variables[2]));
}

bool Player::operator<(Player const &other)
{
    return this->getPercentage() < other.getPercentage();
}

bool Player::operator==(Player const &other)
{
    if(this->getName() == other.getName() &&
            this->getLevel() == other.getLevel() &&
            this->getPercentage() == other.getPercentage())
        return true;
    else
        return false;
}
