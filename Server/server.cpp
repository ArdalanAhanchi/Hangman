
#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include <netdb.h>        // gethostbyname
#include <unistd.h>       // read, write, close
#include <strings.h>      // bzero
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <sys/uio.h>      // writev
#include <stdlib.h>       // atoi
#include <stdio.h>        /* printf, fgets */
#include <iostream>       // IO
#include <fstream>        // File Stream
#include <pthread.h>      // pthread
#include <stdexcept>      //Excpetions
#include <sys/time.h>     // time
#include <vector>
#include <string>
#include <map>

#include "user.h"
#include "room.h"
#include "player.h"
#include "constants.h"

#define PORT 1375

using namespace std;

//Forward declarations
void* threadRequestHandler(void *vptr_value);
void identifyOp(int opCode, string user, int fd);
void writeStat(int status, int fd);
int readOp(int fd);
string readString(int fd);
void writeString(string toWrite, int fd);
int readInt(int fd);
void writeInt(int toWrite, int fd);

//Variables.
map<int, Room> rooms;
map<string, User> users;
int roomIdCounter;

void startServer()
{
    roomIdCounter = 1;

    pthread_t thread1;
    // Declare a sockaddr_in structure
    sockaddr_in acceptSockAddr;

    // zero-initialize it by calling bzero
    bzero((char*) &acceptSockAddr, sizeof(acceptSockAddr));

    // and set its data members
    acceptSockAddr.sin_family = AF_INET; // Address Family Internet
    acceptSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    acceptSockAddr.sin_port = htons(PORT);


    //create socket
    int serverSd = -1;
    serverSd = socket( AF_INET, SOCK_STREAM, 0 );

    // socket is opened
    if(serverSd < 0)
        perror("Cannot open socket");

    // Set the SO_REUSEADDR option. (Note this option is useful to prompt OS to release the server port as soon as your server process is terminated.)
    const int on = 1;
    setsockopt( serverSd, SOL_SOCKET, SO_REUSEADDR, (char *)&on,
                sizeof( int ) );

    // Bind this socket to its local address by calling bind as passing the following arguments: the socket descriptor,
    // the sockaddr_in structure defined above, and its data size.
    bind(serverSd, (sockaddr * ) & acceptSockAddr, sizeof(acceptSockAddr));

    //Looping for new clients attemping to connect
    while (true)
    {
        listen(serverSd, 5);

        sockaddr_in newSockAddr;
        socklen_t newSockAddrSize = sizeof(newSockAddr);
        int newSd = accept(serverSd, (sockaddr * ) & newSockAddr,
                           &newSockAddrSize);
        int arr[1] = {newSd};
        pthread_create(&thread1, NULL, threadRequestHandler, (void *) arr);
    }
    close(serverSd);
}

void writeStat(int status, int fd)
{
    char stat = static_cast<char>(status);
    write(fd, &stat, sizeof(stat));
}

int readOp(int fd)
{
    char op;
    read(fd, &op, sizeof(op));

    return static_cast<int>(op);
}

string readString(int fd)
{
    int strSize = 0;
    read(fd, &strSize, sizeof(strSize));

    char* buffer = new char[strSize];
    read(fd, &buffer, sizeof(buffer));

    string toReturn(buffer);
    delete[] buffer;

    return toReturn;
}

void writeString(string toWrite, int fd)
{
    int strSize = static_cast<char>(toWrite.size());
    write(fd, &strSize, sizeof(strSize));
    write(fd, toWrite.c_str(), toWrite.size());
}

void writeInt(int toWrite, int fd)
{
    write(fd, &toWrite, sizeof(toWrite));
}

int readInt(int fd)
{
    int toReturn = 0;
    read(fd, &toReturn, sizeof(toReturn));

    return toReturn;
}

void identifyOp(int opCode, string user, int fd)
{
    switch(opCode)
    {
        case OP_JOIN:
        {
            //If no rooms exist, create one.
            //if(rooms.size() == 0)
            //    rooms[roomIdCounter] = Room(roomIdCounter++);

            //Find the room with the minimum of players.
            int minRoomId = 1;
            //for(auto r: rooms)
            {
                //if(r.second.getNumPlayers() < rooms[minRoomId].getNumPlayers())
                //    minRoomId = r.second.getRoomID();
            }

            //Join the player in that room.
            Player newPlayer(user);
            rooms[minRoomId].addPlayer(newPlayer);

            break;
        }

        case OP_JOIN_RID:
            break;
        case OP_HOST:
            break;
        case OP_CHECK_GAMEOVER:
            break;
        case OP_GET_PLAYERS:
            break;
        case OP_WON:
            break;
        case OP_LOST:
            break;
        case OP_GET_WORD:
            break;
        case OP_GET_BOARD:
            break;
        case OP_UPDATE:
            break;
        case OP_GET_ROOMS:
            break;
        case OP_GET_PLAYER_COUNT:
            break;

        //If its an unIdentified Operation.
        default:
            writeStat(S_ERROR, fd);
            break;
    }
}

// A function which runs when client gets connected.
void* threadRequestHandler(void *vptr_value)
{
    int *arr = (int *) vptr_value;
    int fd = arr[1];

    string user;
    bool loggedOut = false;
    while(!loggedOut)
    {
        int opCode = static_cast<int>(readOp(fd));

        switch(opCode)
        {
            //If we're creating an account.
            case OP_CREATE_ACT:
            {
                string userInput = readString(fd);
                string passInput = readString(fd);

                int status = S_OK;
                if(userInput == "" || passInput == "")
                {
                    //TODO: Check for invalid user/pass
                    status = S_INVALID_USER_PASS;
                }
                else
                {
                    User newUser(userInput, passInput);
                    //users[userInput] = newUser;
                }

                writeStat(status, fd);
                break;
            }

            //If we're logging in.
            case OP_LOGIN:
            {
                string userInput = readString(fd);
                string passInput = readString(fd);

                int status = S_OK;
                if(userInput == "" || passInput == "")
                {
                    //TODO: Check for invalid user/pass
                    status = S_INVALID_USER_PASS;
                }

                /*if(users.count(userInput))  //If the username exists
                {
                    if(users[userInput].auth(userInput, passInput))     //Check username and password.
                    {
                        users[userInput].login();                       //If right, login the user.
                        user = userInput;
                    }
                }*/
                else
                {
                   status = S_INVALID_USER_PASS;
                }

                writeStat(status, fd);
                break;
            }


            //If there are any other opcodes (The rest of them require the user to be logged in.)
            default:
            {
                if(user == string())                    //If the user is not logged in.
                {
                    writeStat(S_NOT_LOGGED_IN, fd);
                }                                       //If the user is logged in.
                else
                {
                    if(opCode == OP_LOGOUT)             //If we're loggin out.
                    {
                        //users[user].logout();           //logout the user.
                        //users[user].left();             //Leave from the rooms.
                        loggedOut = true;
                        writeStat(S_OK, fd);
                    }
                    else if(opCode == OP_UNREGISTER)
                    {
                        //users.erase(user);           //Remove the user.
                        loggedOut = true;
                        writeStat(S_OK, fd);
                    }
                    else
                    {
                        identifyOp(opCode, user, fd);   //Idefntify opcodes.
                    }
                }
                break;
            }
        }
    }

    shutdown(fd, SHUT_RDWR);
    close(fd);

    return 0;
}

int main(int argc, char *argv[])
{
    cerr << "Starting the Hangman Server" << endl;
    startServer();
}
