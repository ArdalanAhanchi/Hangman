#ifndef CONNECTION_H
#define CONNECTION_H

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

#define HOST "127.0.0.1"
#define PORT 21754
#define MAX_RETRY 10
#define REFRESH_TIME 500
#define NOT_CONNECTED -1

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
    int reg(string user, string pass);
    int login(string user, string pass);
    int logout();
    int joinRand(int &roomId);
    int join(int roomId);
    int host(int &roomId);
    int check(int roomId);
    int getPlayers(int roomId, vector<Player> &players);
    int won(int roomId);
    int lost(int roomId);
    int getWord(int roomId, string &word);
    int getPlayerBoard(int roomId, string &pbString);
    int update(int roomId, Player &toUpdate);
    int getRooms(RoomList &rooms);
    int getPlayerCount(int roomId, int& count);
    int unregister();

    void close();

private:
    void writeOp(int status);
    void writeString(string toWrite);
    void writeInt(int toWrite);

    string readString();
    int readStat();
    int readInt();

    //A function for printing not ok status.
    void notOk(int status);

    //For reconnecting if the server is disconnected.
    bool checkConnection();

    QTcpSocket* socket;
    int fd;
};

#endif // CONNECTION_H
