#ifndef ROOM_H
#define ROOM_H

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
    int getRoomId();
    int getNumPlayers();
    string getPlayerBoard();
    string getWord();

    // Accessors
    void addPlayer(Player &toAdd);
    void removePlayer(Player &toRemove);
    void update(Player &toUpdate);

    void won(Player &winner);
    void lost(Player &loser);

    bool isOver();

private:
    int roomId;
    vector<Player> players;
    string word;
    bool gameOver;
    string winnerName;

    string generateWord();
};

#endif /* Room_hpp */
