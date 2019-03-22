#ifndef ROOMLIST_H
#define ROOMLIST_H

#include <vector>
#include <string>
#include <iostream>

//  Ardalan Ahanchi
//  CSS 432
//  Hangman

#define INVALID_ROOM_CHOICE -1

using namespace std;

//A class for holding the rooms list which were retrieved from the server.
class RoomList
{
public:
    //Default constructor.
    RoomList();

    //Add a room to the list.
    void addRoom(int roomId, int numPlayers);

    //Get a string of rooms.
    string toString();

    //Returns the roomId for the given choice.
    int choose(int choice);

    //Returns the choice for hosting a new game.
    int newGameChoice();

    //Returns the number of rooms.
    int getSize();

    //Empty the rooms list.
    void clear();

private:
    vector<int> roomIds;                //For holding roomId/numberOfPlayers.
    vector<int> roomNumPlayers;
};

#endif // ROOMLIST_H
