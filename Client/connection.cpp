#include "connection.h"

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

int Connection::reg(string user, string pass)
{
    cerr << "Connection: Registering new client." << endl;

    if(!checkConnection())
        return NOT_CONNECTED;

    writeOp(OP_CREATE_ACT);
    writeString(user);
    writeString(pass);
    return readStat();
}

int Connection::login(string user, string pass)
{
    if(!checkConnection())
        return NOT_CONNECTED;

    writeOp(OP_LOGIN);
    writeString(user);
    writeString(pass);
    return readStat();
}

int Connection::logout()
{
    if(!checkConnection())
        return NOT_CONNECTED;

    writeOp(OP_LOGOUT);
    return readStat();
}

int Connection::joinRand(int &roomId)
{
    if(!checkConnection())
        return NOT_CONNECTED;

    writeOp(OP_JOIN);
    int status = readStat();

    if(status == S_OK)
        roomId = readInt();
    else
        notOk(status);

    return status;
}

int Connection::join(int roomId)
{
    if(!checkConnection())
        return NOT_CONNECTED;

    writeOp(OP_JOIN_RID);
    writeInt(roomId);
    return readStat();
}

int Connection::host(int &roomId)
{
    if(!checkConnection())
        return NOT_CONNECTED;

    writeOp(OP_HOST);
    int status = readStat();
    if(status == S_OK)
        roomId = readInt();
    else
        notOk(status);

    return status;
}

int Connection::check(int roomId)
{
    if(!checkConnection())
        return NOT_CONNECTED;

    writeOp(OP_CHECK_GAMEOVER);
    writeInt(roomId);
    return readStat();
}

int Connection::getPlayers(int roomId, vector<Player> &players)
{
    if(!checkConnection())
        return NOT_CONNECTED;

    //Write the opcode.
    writeOp(OP_GET_PLAYERS);

    //Write the room number.
    writeInt(roomId);

    int status = readStat();
    if(status == S_OK)
    {
        //Read the number of elements, and then initialize the players.
        int numberOfElements = readInt();
        for(int i = 0; i < numberOfElements; i++)
        {
            Player newPlayer;
            newPlayer.deserialize(readString());
            players.push_back(newPlayer);
        }
    }
    else { notOk(status); }

    return status;
}

int Connection::won(int roomId)
{
    if(!checkConnection())
        return NOT_CONNECTED;

    writeOp(OP_WON);
    writeInt(roomId);
    return readStat();
}

int Connection::lost(int roomId)
{
    if(!checkConnection())
        return NOT_CONNECTED;

    writeOp(OP_LOST);
    writeInt(roomId);
    return readStat();
}

int Connection::getWord(int roomId, string &word)
{
    if(!checkConnection())
        return NOT_CONNECTED;

    writeOp(OP_GET_WORD);
    writeInt(roomId);
    int status = readStat();

    if(status == S_OK)
        word = readString();
    else
        notOk(status);

    return status;
}

int Connection::getPlayerBoard(int roomId, string &pbString)
{
    if(!checkConnection())
        return NOT_CONNECTED;

    writeOp(OP_GET_BOARD);
    writeInt(roomId);
    int status = readStat();

    if(status == S_OK)
        pbString = readString();
    else
        notOk(status);

    return status;
}

int Connection::update(int roomId, Player &toUpdate)
{
    if(!checkConnection())
        return NOT_CONNECTED;

    writeOp(OP_UPDATE);
    writeInt(roomId);
    writeString(toUpdate.serialize());
    return readStat();
}

int Connection::getRooms(RoomList &rooms)
{
    if(!checkConnection())
        return NOT_CONNECTED;

    writeOp(OP_GET_ROOMS);
    int status = readStat();

    if(status == S_OK)
    {
        //Read the number of rooms, and then add each room to rooms.
        int numberOfElements = readInt();
        cerr << "The number of elements is : " << numberOfElements << endl;
        for(int i = 0; i < numberOfElements; i++)
        {
            int roomId = readInt();
            int roomNumPlayers = readInt();
            rooms.addRoom(roomId, roomNumPlayers);
        }
            //rooms.addRoom(readInt(), readInt());            //Read two integers from server which represent roomId/roomNumber.
    }
    else { notOk(status); }

    return status;
}

int Connection::getPlayerCount(int roomId, int &count)
{
    if(!checkConnection())
        return NOT_CONNECTED;

    writeOp(OP_GET_PLAYER_COUNT);
    writeInt(roomId);
    int status = readStat();

    if(status == S_OK)
        count = readInt();

    return status;
}

int Connection::unregister()
{
    if(!checkConnection())
        return NOT_CONNECTED;

    writeOp(OP_UNREGISTER);
    return readStat();
}

void Connection::close()
{
    socket->close();
    delete socket;
}

void Connection::writeOp(int status)
{
    char op = static_cast<char>(status);
    socket->waitForBytesWritten();
    socket->flush();

    while(!socket->isWritable())
        cerr << "Server: WriteOP error. Trying again." << endl;

    socket->write(&op, sizeof(op));
}

void Connection::writeString(string toWrite)
{
    int strSize = static_cast<char>(toWrite.size());
    socket->waitForBytesWritten();
    socket->flush();

    while(!socket->isWritable())
        cerr << "Server: WriteString error. Trying again." << endl;

    writeInt(strSize);
    socket->write(toWrite.c_str());
}

int Connection::readStat()
{
    socket->waitForReadyRead();         //Wait till the socket is readable.
    char status;
    socket->read(&status, sizeof(status));
    socket->read(&status, sizeof(status));
    cerr << "Connection: Status retured was " << static_cast<int>(status) << endl; //status << endl;
    return static_cast<int>(status);
}

string Connection::readString()
{
    while(!socket->isReadable()) {}
    int strSize = readInt();

    socket->waitForReadyRead();
    char* buffer = new char[strSize];
    socket->read(buffer, strSize);

    string toReturn(buffer);
    delete[] buffer;

    return toReturn;
}

void Connection::writeInt(int toWrite)
{
    //QByteArray dataArr;
    //QDataStream dataStream(&dataArr, QIODevice::WriteOnly);
    //dataStream << toWrite;
    //socket->write(dataArr);
    socket->write(reinterpret_cast<const char*>(&toWrite), sizeof(toWrite));
    socket->waitForBytesWritten();
    socket->flush();
}

int Connection::readInt()
{
    socket->waitForReadyRead();
    int toReturn = 0;
    socket->read(reinterpret_cast<char*>(toReturn), sizeof(toReturn));
    cerr << "Returned value is:" << toReturn << " " << ntohl(toReturn) << endl;
    return toReturn;
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
