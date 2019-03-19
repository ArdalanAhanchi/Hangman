#include "Room.hpp"

Room::Room(int roomNum, Player playersInRoom[]) {
    if (roomNum != NULL && playersInRoom != NULL) {
        this.roomID = roomNum;
        this.players = playersInRoom;
        this.word = generateWord();
    }
}

int Room::getRoomID() {
    return roomID;
}

int Room::getNumPlayers() {
    return players.size();
}

PlayerBoard Room::getPlayerBoard() {
//
    return PlayerBoard;
}

string Room::generateWord() {
    string retWord;
    int random = rand() % 200;
    ifstream input( "wordList.txt" );
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            if(random = numLines) {
                retWord = line.c_str();
                break;
            }
            numLines++;
        }
        file.close();
    }
    return word;
}
