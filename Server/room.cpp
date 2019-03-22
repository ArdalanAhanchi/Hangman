#include "room.h"

Room::Room()
{
    roomId = 0;
    word = generateWord();
    gameOver = false;
    winnerName = "";
}

Room::Room(int roomNum)
{
    if (roomNum != 0)
    {
        roomId = roomNum;
        word = generateWord();
        gameOver = false;
        winnerName = "";
    }
}

Room::Room(const Room& copy)
{
    roomId = copy.roomId;
    word = copy.word;
    players = copy.players;
    gameOver = copy.gameOver;
    winnerName = copy.winnerName;
}

int Room::getRoomId()
{
    return roomId;
}

int Room::getNumPlayers()
{
    return players.size();
}

string Room::getWord()
{
    return word;
}

string Room::getPlayerBoard()
{
    //Print the number of players.
    string pbString = "Number Of Players In Room: ";
    pbString += players.size();
    pbString += "\n";

    if(gameOver)
    {
        pbString += "Player ";
        pbString += winnerName;
        pbString += " Just won the Game!\n";
    }

    //Print the PLAYER_BOARD_MAX players list.
    std::sort(players.begin(), players.end());
    int num = ( players.size() < PLAYER_BOARD_MAX ? players.size() : PLAYER_BOARD_MAX );
    for(int i = 0; i < num; i++)
    {
        pbString += to_string(i + 1);
        pbString += string(". ");
        pbString += players[i].getName();
        pbString += string("    :    ");
        pbString += to_string(players[i].getPercentage());
        pbString += string("\n");
    }

    return pbString;
}

string Room::generateWord()
{
    string retWord;
    int random = rand() % WORD_NUM_LINES;
    int numLines = 0;
    ifstream input(WORD_FILE_NAME);
    if (input.is_open())
    {
        string line;
        while (getline(input, line))
        {
            if(random = numLines++)
            {
                retWord = string(line.c_str());
                break;
            }
        }
        input.close();
    }
    return retWord;
}

void Room::addPlayer(Player &toAdd)
{
    //Return if player was already there.
    for(Player p: players)
        if(p == toAdd)
            return;

    players.push_back(toAdd);
}

void Room::removePlayer(Player &toRemove)
{
    //Erase the player if it was found.
    for(int i = 0; i < players.size(); i++)
    {
        if(players[i] == toRemove)
            players.erase(players.begin() + i);
    }
}

void Room::update(Player &toUpdate)
{
    if(gameOver)
        return;

    for(Player p: players)
    {
        if(p.getName() == toUpdate.getName())
        {
            p.setLevel(toUpdate.getLevel());
            p.setPercentage(toUpdate.getPercentage());
        }
    }
}

void Room::won(Player &winner)
{
    for(Player p: players)
    {
        if(p.getName() == winner.getName())
        {
            winnerName = p.getName();
            gameOver = true;
        }
    }
}

void Room::lost(Player &loser)
{
    removePlayer(loser);
}

bool Room::isOver()
{
    return gameOver;
}
