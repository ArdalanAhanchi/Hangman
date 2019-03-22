#include "connection.h"

//  Ardalan Ahanchi
//  CSS 432
//  Hangman

Connection::Connection(QObject* parent) : QObject(parent)
{
    connect();
}

Connection::~Connection()
{
    //Properly handle the socket on exit.
    logout();
    close();
}

void Connection::connect()
{
    socket = new QTcpSocket(this);
    socket->connectToHost(HOST, PORT);

    if(socket->waitForConnected(3000))
        cerr << "Connection: Connected to Server." << endl;
    else
        cerr << "Connection: Couldn't connect to server." << endl;
}

void Connection::writePacket(Packet &p)
{
    vector<char> serialized;
    p.serialize(serialized);

    if(socket->write(&serialized[0] , serialized.size()) < 1)
        cerr << "Write packet failed" << endl;
}

void Connection::readPacket(Packet &p)
{
    char buffer[10];
    socket->waitForReadyRead();
    socket->bytesAvailable();

    if(socket->read(buffer, 10) < 1)
        cerr << "Read Packet failed" << endl;

    vector<char> serialized;
    for(int i = 0; i < 10; i++)
    {
        serialized.push_back(buffer[i]);
        cerr << buffer[i] ;
    }

    p = Packet(serialized);
}

char Connection::reg(string user, string pass)
{
    if(!checkConnection())
        return NOT_CONNECTED;

    Packet toSend(OP_CREATE_ACT);
    toSend.addArg(user);
    toSend.addArg(pass);

    writePacket(toSend);

    Packet toRecieve;
    readPacket(toRecieve);

    return toRecieve.getOpCode();
}

char Connection::login(string user, string pass)
{
    if(!checkConnection())
        return NOT_CONNECTED;

    Packet toSend(OP_LOGIN);
    toSend.addArg(user);
    toSend.addArg(pass);

    writePacket(toSend);

    Packet toRecieve;
    readPacket(toRecieve);

    return toRecieve.getOpCode();
}

char Connection::logout()
{
    if(!checkConnection())
        return NOT_CONNECTED;

    Packet toSend(OP_LOGOUT);
    writePacket(toSend);

    Packet toRecieve;
    readPacket(toRecieve);

    return toRecieve.getOpCode();
}

char Connection::joinRand(int &roomId)
{
    if(!checkConnection())
        return NOT_CONNECTED;

    Packet toSend(OP_JOIN);                 //Send request
    writePacket(toSend);

    Packet toRecieve;                       //Recieve the packet.
    readPacket(toRecieve);

    if(toRecieve.getNumArgs() == 1)         //Recieve the new roomId.
        roomId = stoi(toRecieve.getArg(0));

    return toRecieve.getOpCode();           //Return status.
}

char Connection::join(int roomId)
{
    if(!checkConnection())
        return NOT_CONNECTED;

    Packet toSend(OP_JOIN_RID);             //Send room id to server.
    toSend.addArg(to_string(roomId));
    writePacket(toSend);

    Packet toRecieve;                       //Recieve the packet.
    readPacket(toRecieve);

    return toRecieve.getOpCode();           //Return status.
}

char Connection::host(int &roomId)
{
    if(!checkConnection())
        return NOT_CONNECTED;

    Packet toSend(OP_HOST);                 //send request.
    writePacket(toSend);

    Packet toRecieve;
    readPacket(toRecieve);

    if(toRecieve.getNumArgs() == 1)         //Recieve the new roomId.
        roomId = stoi(toRecieve.getArg(0));

    return toRecieve.getOpCode();
}

char Connection::check(int roomId)
{
    if(!checkConnection())
        return NOT_CONNECTED;

    Packet toSend(OP_CHECK_GAMEOVER);                 //send request.
    toSend.addArg(to_string(roomId));
    writePacket(toSend);

    Packet toRecieve;
    readPacket(toRecieve);

    return toRecieve.getOpCode();
}

char Connection::won(int roomId)
{
    if(!checkConnection())
        return NOT_CONNECTED;

    Packet toSend(OP_WON);                 //send request.
    toSend.addArg(to_string(roomId));
    writePacket(toSend);

    Packet toRecieve;
    readPacket(toRecieve);

    return toRecieve.getOpCode();
}

char Connection::lost(int roomId)
{
    if(!checkConnection())
        return NOT_CONNECTED;

    Packet toSend(OP_LOST);                 //send request.
    toSend.addArg(to_string(roomId));
    writePacket(toSend);

    Packet toRecieve;
    readPacket(toRecieve);

    return toRecieve.getOpCode();
}

char Connection::getWord(int roomId, string &word)
{
    if(!checkConnection())
        return NOT_CONNECTED;

    Packet toSend(OP_GET_WORD);
    toSend.addArg(to_string(roomId));
    writePacket(toSend);

    Packet toRecieve;
    readPacket(toRecieve);

    if(toRecieve.getNumArgs() == 1)
        word = toRecieve.getArg(0);

    return toRecieve.getOpCode();
}

char Connection::getPlayerBoard(int roomId, string &pbString)
{
    if(!checkConnection())
        return NOT_CONNECTED;

    Packet toSend(OP_GET_BOARD);
    toSend.addArg(to_string(roomId));
    writePacket(toSend);

    Packet toRecieve;
    readPacket(toRecieve);

    if(toRecieve.getNumArgs() == 1)
        pbString = toRecieve.getArg(0);

    return toRecieve.getOpCode();
}

char Connection::update(int roomId, Player &toUpdate)
{
    if(!checkConnection())
        return NOT_CONNECTED;

    Packet toSend(OP_UPDATE);
    toSend.addArg(to_string(roomId));               //Add room number and player.
    toSend.addArg(toUpdate.serialize());
    writePacket(toSend);                            //Send the update packet.

    Packet toRecieve;
    readPacket(toRecieve);

    return toRecieve.getOpCode();                   //Return status code.
}

char Connection::leave()
{
    if(!checkConnection())
        return NOT_CONNECTED;

    Packet toSend(OP_LEAVE);
    writePacket(toSend);

    Packet toRecieve;
    readPacket(toRecieve);
    return toRecieve.getOpCode();
}

char Connection::getRooms(RoomList &rooms)
{
    if(!checkConnection())
        return NOT_CONNECTED;

    Packet toSend(OP_GET_ROOMS);
    writePacket(toSend);

    Packet toRecieve;
    readPacket(toRecieve);

    for(int i = 0; i < toRecieve.getNumArgs(); i++)
    {
        rooms.addRoom(stoi(toRecieve.getArg(i)), 0);
    }

    return toRecieve.getOpCode();
}

char Connection::getPlayerCount(int roomId, int &count)
{
    if(!checkConnection())
        return NOT_CONNECTED;

    Packet toSend(OP_GET_PLAYER_COUNT);
    toSend.addArg(to_string(roomId));
    writePacket(toSend);

    Packet toRecieve;
    readPacket(toRecieve);

    if(toRecieve.getNumArgs() == 1)
        count = stoi(toRecieve.getArg(0));

    return toRecieve.getOpCode();
}

char Connection::unregister()
{
    if(!checkConnection())
        return NOT_CONNECTED;

    Packet toSend(OP_UNREGISTER);
    writePacket(toSend);

    Packet toRecieve;
    readPacket(toRecieve);

    return toRecieve.getOpCode();
}

void Connection::close()
{
    socket->close();
    delete socket;
}


void Connection::notOk(int status)
{
   cerr << "Connection: ERROR: Server returned a " << status << endl;
}

bool Connection::checkConnection()
{
    int numRetrys = 0;
    while(socket->state() != QTcpSocket::ConnectedState && numRetrys < MAX_RETRY)
    {
        usleep(REFRESH_TIME);
        cerr << "Connection: ERROR: Server disconnected, reconnecting..." << endl;
        connect();
        numRetrys++;
    }

    if(numRetrys == MAX_RETRY)
        return false;

    return true;
}
