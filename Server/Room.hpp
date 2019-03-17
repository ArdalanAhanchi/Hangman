#ifndef Room_hpp
#define Room_hpp

#include <string>
#include <iostream>
#include <stdio.h>

using namespace std;

class Room {
public:
    // Constructors
    Room();
    
    // Getters
    int getRoomID();
    int getNumPlayers();
    PlayerBoard getPlayerBoard();
    
private:
    int roomID;
    Player players[];
    string word;

}

#endif /* Room_hpp */
