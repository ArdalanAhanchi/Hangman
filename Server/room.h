#ifndef Room_hpp
#define Room_hpp

#include <fstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <algorithm>

#include "player.h"

#define PLAYER_BOARD_MAX 5

#define WORD_FILE_NAME "wordList.txt"
#define WORD_NUM_LINES 200

using namespace std;

class Room {
public:
    // Constructors
    Room();
    Room(int roomNum);
    Room(const Room& copy);

    // Getters
    int getRoomID();
    int getNumPlayers();
    string getPlayerBoard();
    string getWord();

    // Accessors
    void addPlayer(Player &toAdd);
    void removePlayer(Player &toRemove);
    void update(Player &toUpdate);

private:
    int roomID;
    vector<Player> players;
    string word;

    string generateWord();
};

#endif /* Room_hpp */
