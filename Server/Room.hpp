#ifndef Room_hpp
#define Room_hpp

#include <fstream>
#include <string>
#include <iostream>
#include <stdio.h>

using namespace std;

class Room {
public:
    // Constructors
    Room(int roomNum, Player playersInRoom[]);
    
    // Getters
    int getRoomID();
    int getNumPlayers();
    PlayerBoard getPlayerBoard();

    string generateWord();
private:
    int roomID;
    Player players[];
    string word;
}

#endif /* Room_hpp */
