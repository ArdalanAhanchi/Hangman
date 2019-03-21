#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include <netdb.h>        // gethostbyname
#include <unistd.h>       // read, write, close, usleep
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
#include <mutex>

#include "user.h"
#include "room.h"
#include "player.h"
#include "constants.h"

#define PORT 21754
#define MAX_CONNECTIONS 30
#define REFRESH_TIME 2000
#define MIN_PASSWORD_LENGTH 8

using namespace std;

//Forward declarations
void* connected(void* args);
void identifyOp(int opCode, string user, int fd);
void writeStat(int status, int fd);
int readOp(int fd);
string readString(int fd);
void writeString(string toWrite, int fd);
int readInt(int fd);
void writeInt(int toWrite, int fd);
void disconnected(int fd);

//Variables.
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
map<int, Room> rooms;
map<string, User> users;
int roomIdCounter;

void startServer()
{
    roomIdCounter = 1;

    sockaddr_in socketAddress;                                                  //Create a socket address.
    bzero((char*) &socketAddress, sizeof(socketAddress));
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    socketAddress.sin_port = htons(PORT);

    int serverFd = -1;
    serverFd = socket(AF_INET, SOCK_STREAM, 0);                                 //Initialize socket

    if(serverFd < 0)                                                            // socket is opened
        cerr << "ERROR: Cannot open socket" << endl;

    const int on = 1;
    setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, (char*) &on, sizeof(int));   // Set the SO_REUSEADDR option.

    bind(serverFd, (sockaddr * ) & socketAddress, sizeof(socketAddress));       // Bind this socket to its local address.
    listen(serverFd, MAX_CONNECTIONS);

    //Looping for new clients attemping to connect
    while (true)
    {
        sockaddr_in clientAddr;                                                 // Create a client socket.
        socklen_t clientAddrSize = sizeof(clientAddr);
        int clientFd = accept(serverFd, (sockaddr*) &clientAddr, &clientAddrSize);  // Wait for a connection.
        int* args = new int[1] {clientFd};                                      // Create an arguments array.
        pthread_t clientThread;
        pthread_create(&clientThread, NULL, connected, (void*) args);           // Start a thread for the client.
    }

    close(serverFd);
}

void disconnected(int fd)
{
    cerr << "Client Disconnected." << endl;
    close(fd);
    pthread_cancel(pthread_self());
}

void writeStat(int status, int fd)
{
    cerr << "Writing status + " << to_string(status) << endl;
    char stat = static_cast<char>(status);
    if(write(fd, &stat, sizeof(stat)) <= 0)
        disconnected(fd);
}

int readOp(int fd)
{
    char op;
    if(read(fd, &op, sizeof(op)) <= 0)
        disconnected(fd);

    return static_cast<int>(op);
}

string readString(int fd)
{
    int strSize = 0;
    if(read(fd, &strSize, sizeof(strSize)) <= 0)
        disconnected(fd);

    char* buffer = new char[strSize];
    if(read(fd, buffer, strSize) <= 0)
        disconnected(fd);

    string toReturn(buffer);
    delete[] buffer;

    return toReturn;
}

void writeString(string toWrite, int fd)
{
    int strSize = static_cast<char>(toWrite.size());

    if(write(fd, &strSize, sizeof(strSize)) <= 0)
        disconnected(fd);

    if(write(fd, toWrite.c_str(), toWrite.size()))
        disconnected(fd);
}

void writeInt(int toWrite, int fd)
{
    cerr << "Writing Integer + " << to_string(toWrite) << endl;
    int converted = htonl(toWrite);
    if(write(fd, &converted, sizeof(converted)) <= 0)
        disconnected(fd);
}

int readInt(int fd)
{
    int toReturn = 0;
    if(read(fd, &toReturn, sizeof(toReturn)) <= 0)
        disconnected(fd);

    return toReturn;
}

//Opcodes which require the user to be logged in.
void identifyOp(int opCode, string user, int fd)
{
    switch(opCode)
    {
        case OP_JOIN:
        {
            pthread_mutex_lock(&mtx);

            //If no rooms exist, create one.
            if(rooms.size() == 0)
                rooms[roomIdCounter] = Room(roomIdCounter++);

            //Find the room with the minimum of players.
            int minRoomId = 1;
            for(auto r: rooms)
            {
                if(r.second.getNumPlayers() < rooms[minRoomId].getNumPlayers())
                    minRoomId = r.second.getRoomId();
            }

            //Join the player in that room.
            Player newPlayer(user);
            rooms[minRoomId].addPlayer(newPlayer);
            pthread_mutex_unlock(&mtx);

            break;
        }

        case OP_JOIN_RID:
        {
            //pthread_mutex_lock(&mtx);
            //pthread_mutex_unlock(&mtx);
            break;
        }

        case OP_HOST:
            //pthread_mutex_lock(&mtx);
            //pthread_mutex_unlock(&mtx);
            break;
        case OP_CHECK_GAMEOVER:
            //pthread_mutex_lock(&mtx);
            //pthread_mutex_unlock(&mtx);
            break;
        case OP_GET_PLAYERS:
            //pthread_mutex_lock(&mtx);
            //pthread_mutex_unlock(&mtx);
            break;
        case OP_WON:
            //pthread_mutex_lock(&mtx);
            //pthread_mutex_unlock(&mtx);
            break;
        case OP_LOST:
            //pthread_mutex_lock(&mtx);
            //pthread_mutex_unlock(&mtx);
            break;
        case OP_GET_WORD:
            //pthread_mutex_lock(&mtx);
            //pthread_mutex_unlock(&mtx);
            break;
        case OP_GET_BOARD:
            //pthread_mutex_lock(&mtx);
            //pthread_mutex_unlock(&mtx);
            break;
        case OP_UPDATE:
            //pthread_mutex_lock(&mtx);
            //pthread_mutex_unlock(&mtx);
            break;

        case OP_GET_ROOMS:
        {

            writeStat(S_OK, fd);                                                //Write the OK status.

            //pthread_mutex_lock(&mtx);
            writeInt(rooms.size() + 1, fd);                                         //Write the number of rooms.
            for(auto r: rooms)
            {
                writeInt(r.second.getRoomId(), fd);                             //Write the roomId + number of players to socket.
                writeInt(r.second.getNumPlayers(), fd);
            }
            //pthread_mutex_unlock(&mtx);
            break;
        }

        case OP_GET_PLAYER_COUNT:
            //pthread_mutex_lock(&mtx);
            //pthread_mutex_unlock(&mtx);
            break;

        //If its an unIdentified Operation.
        default:
            cerr << "I get to unidentified opcode." << endl;
            writeStat(S_ERROR, fd);
            break;
    }
}

// A function which runs when client gets connected.
void* connected(void* args)
{
    cerr << "New Client Connected." << endl;
    int* arr = (int*) args;
    int fd = arr[0];                                                            //Get the Fd.

    string user;
    bool loggedOut = false;
    while(!loggedOut)
    {
        usleep(REFRESH_TIME);
        int opCode = readOp(fd);
        if(opCode <= 0)                                                         //Exit if the connection was dropped by client.
            break;

        //cerr << "Server: Opcode requested was: " << opCode << endl;
        switch(opCode)
        {
            //If we're creating an account.
            case OP_CREATE_ACT:
            {
                string userInput = readString(fd);                              //Read the strings from client.
                string passInput = readString(fd);

                int status = S_OK;
                if(userInput == "")
                {
                    status = S_REG_INVALID_USER;                                //If username is empty.
                }
                else if (passInput.length() < MIN_PASSWORD_LENGTH)
                {
                    status = S_REG_INVALID_PASS;                                //If password is invalid.
                }
                else
                {
                    //pthread_mutex_lock(&mtx);
                    if (users.find(userInput) == users.end() )                  //If its a new user.
                    {
                        cerr << "Creating a new account: Username = " << userInput << endl;
                        User newUser(userInput, passInput);
                        users[userInput] = newUser;
                    }
                    else                                                        //If its an existing user.
                    {
                        cerr << "Username existed, Not Creating Account." << endl;
                        status = S_REG_USERNAME_EXISTS;
                    }
                    //pthread_mutex_unlock(&mtx);
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
                if(userInput == "" || passInput == "")                          //Check for empty user/pass
                    status = S_AUTH_INVALID_USER;

                //pthread_mutex_lock(&mtx);
                if(users.count(userInput))                                      //If the username exists
                {
                    if(users[userInput].auth(userInput, passInput))             //Check password.
                    {
                        users[userInput].login();                               //If right, login the user.
                        user = userInput;
                    }
                    else
                    {
                        status = S_AUTH_INVALID_PASS;                           //If password is wrong.
                    }
                }
                else
                {
                   status = S_AUTH_INVALID_USER;                                //If the username is not registerd.
                }
                //pthread_mutex_unlock(&mtx);

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
                    //pthread_mutex_lock(&mtx);

                    if(opCode == OP_LOGOUT)             //If we're loggin out.
                    {
                        users[user].logout();           //logout the user.
                        users[user].left();             //Leave from the rooms.
                        loggedOut = true;
                        writeStat(S_OK, fd);
                    }
                    else if(opCode == OP_UNREGISTER)
                    {
                        users.erase(user);              //Remove the user.
                        loggedOut = true;
                        writeStat(S_OK, fd);
                    }
                    else
                    {
                        identifyOp(opCode, user, fd);   //Idefntify opcodes.
                    }

                    //pthread_mutex_unlock(&mtx);
                }
                break;
            }
        }
    }
    cerr << "Client Left." << endl;

    delete[] arr;
    shutdown(fd, SHUT_RDWR);
    close(fd);

    return 0;
}

int main(int argc, char *argv[])
{
    cerr << "Starting the Hangman Server" << endl;
    startServer();
}
