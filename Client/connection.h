#ifndef CONNECTION_H
#define CONNECTION_H

//  Ardalan Ahanchi
//  CSS 432
//  Hangman

#include <QTcpSocket>
#include <QtCore>
#include <string>
#include <QObject>
#include <vector>
#include <unistd.h>
#include <netinet/in.h>

#include "constants.h"
#include "roomlist.h"
#include "player.h"
#include "packet.h"

#define HOST "127.0.0.1"
#define PORT 21754
#define MAX_RETRY 10
#define REFRESH_TIME 500
#define NOT_CONNECTED -1

#define PACKET_LIMIT 512

using namespace std;

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(QObject* parent = 0);
    ~Connection();

    //For connecting to the server.
    void connect();

    //Api functions. The parameters passed by reference are return values.
    char reg(string user, string pass);
    char login(string user, string pass);
    char logout();
    char joinRand(int &roomId);
    char join(int roomId);
    char host(int &roomId);
    char check(int roomId);
    char won(int roomId);
    char lost(int roomId);
    char getWord(int roomId, string &word);
    char getPlayerBoard(int roomId, string &pbString);
    char update(int roomId, Player &toUpdate);
    char getRooms(RoomList &rooms);
    char getPlayerCount(int roomId, int& count);
    char unregister();
    char leave();

    void close();

private:
    void writePacket(Packet &p);
    void readPacket(Packet &p);

    //A function for printing not ok status.
    void notOk(int status);

    //For reconnecting if the server is disconnected.
    bool checkConnection();

    QTcpSocket* socket;
};

#endif // CONNECTION_H
