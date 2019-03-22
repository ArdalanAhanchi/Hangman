#include "roomlist.h"

//Default constructor.
RoomList::RoomList() {}

//Add a room to the list.
void RoomList::addRoom(int roomId, int numPlayers)
{
    roomIds.push_back(roomId);
    roomNumPlayers.push_back(numPlayers);
}

//Get a string of rooms.
string RoomList::toString()
{
    string toReturn = "";
    if(roomIds.size() == 0)
        toReturn += "No Rooms Have Been Created In Server. Create a new Room!\n\n";

    for(size_t i = 0; i < roomIds.size(); i++)
    {
        toReturn += to_string(i + 1);
        toReturn += ". Room #";
        toReturn += to_string(roomIds[i]);
        //toReturn += "   |   ";
        //toReturn += to_string(roomNumPlayers[i]);
        //toReturn += " Online Players.\n";
        toReturn += "\n";
    }

    toReturn += to_string(roomIds.size() + 1);
    toReturn += ". Host a new game.\n";
    return toReturn;
}

//Returns the roomId for the given choice.
int RoomList::choose(int choice)
{
    size_t uChoice = static_cast<size_t>(choice);
    if(uChoice < 1 || uChoice > roomIds.size())             //If invalid choice, return proper value and print error.
    {
        cerr << "RoomList: ERROR: Invalid choice" << endl;
        return INVALID_ROOM_CHOICE;
    }

    return roomIds[uChoice - 1];
}

//Returns the number of rooms.
int RoomList::getSize()
{
    return static_cast<int>(roomIds.size());
}

//Returns the choice for hosting a new game.
int RoomList::newGameChoice()
{
    return static_cast<int>(roomIds.size() + 1);
}

//Empty the rooms list.
void RoomList::clear()
{
    roomIds.clear();
    roomNumPlayers.clear();
}
