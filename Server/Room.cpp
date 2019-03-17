#include "Room.hpp"

Room::Room() {
//    roomID = 1;
}

int Room::getRoomID() {
    return roomID;
}

int Room::getNumPlayers() {
    return players.size();
}

PlayerBoard Room::getPlayerBoard() {
    
    return PlayerBoard;
}
