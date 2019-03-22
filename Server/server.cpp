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
#include <iostream>
#include <string>
#include <map>
#include <mutex>

#include "user.h"
#include "room.h"
#include "player.h"
#include "constants.h"
#include "packet.h"

#define PORT 21754
#define MAX_CONNECTIONS 30
#define REFRESH_TIME 2000
#define MIN_PASSWORD_LENGTH 8
#define PACKET_LIMIT 512

using namespace std;

//Forward declarations
void* connected(void* args);
int readPacket(Packet &p, int fd);
int writePacket(Packet &p, int fd);
int writeStat(char status, int fd);
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

//Returns a packet which it read from the fd.
int readPacket(Packet &p, int fd)
{                                                                                //Read from buffer while there is data to read.
    char buffer[PACKET_LIMIT];

    if(read(fd, &buffer, PACKET_LIMIT) < 1)
    {
        disconnected(fd);
        return S_ERROR;
    }

    vector<char> serialized;
    for(char c: buffer)
        serialized.push_back(c);

    p = Packet(serialized);
    return S_OK;
}

//Writes the packet p to the fd.
int writePacket(Packet &p, int fd)
{
    vector<char> serialized;
    p.serialize(serialized);

    if(write(fd, &serialized[0] , serialized.size()) < 1)
    {
        disconnected(fd);
        return S_ERROR;
    }

    return S_OK;
}

//Write the status to the fd.
int writeStat(char status, int fd)
{
    Packet toSend(status);
    return writePacket(toSend, fd);
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
        cerr << "I come to pre readOp" << endl;

        Packet request;
        readPacket(request, fd);
        char opCode = request.getOpCode();
        cerr << "THe requested opcode is : " << opCode << endl;
        if(opCode < 0)
            disconnected(fd);

        switch(opCode)
        {
            //If we're creating an account.
            case OP_CREATE_ACT:
            {
                string userInput = "";
                string passInput = "";
                if(request.getNumArgs() >= 2)
                {
                    userInput = request.getArg(0);                              //Read the strings from client.
                    passInput = request.getArg(1);
                }

                cerr << "Username is: " << userInput << endl;
                cerr << "Passcode is: " << passInput << endl;

                char status = S_OK;
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
                string userInput = "";
                string passInput = "";
                if(request.getNumArgs() >= 2)
                {
                    userInput = request.getArg(0);                              //Read the strings from client.
                    passInput = request.getArg(1);
                }

                int status = S_OK;
                if(userInput == "" || passInput == "")                          //Check for empty user/pass
                    status = S_AUTH_INVALID_USER;

                //pthread_mutex_lock(&mtx);
                if(users.count(userInput))                                      //If the username exists
                {
                    if(users[userInput].auth(userInput, passInput))             //Check password.
                    {
                        users[userInput].login();                               //If right, login the user.
                        user.append(userInput);
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
                if(!users[user].isLoggedIn())                    //If the user is not logged in.
                {
                    cerr << "I get to not logged in somehow for user " << user << endl;
                    writeStat(S_NOT_LOGGED_IN, fd);
                }                                       //If the user is logged in.
                else
                {
                    //pthread_mutex_lock(&mtx);
                    switch(opCode)
                    {
                        case OP_LOGOUT:
                        {
                            users[user].logout();           //logout the user.
                            users[user].left();             //Leave from the rooms.
                            loggedOut = true;
                            writeStat(S_OK, fd);
                            break;
                        }

                        case OP_UNREGISTER:
                        {
                            users[user].logout();           //logout the user.
                            users[user].left();             //Leave from the rooms.
                            loggedOut = true;

                            users.erase(users.find(user));  //Remove from users.
                            writeStat(S_OK, fd);
                            break;
                        }

                        case OP_JOIN:
                        {
                            //If no rooms exist, create one.
                            if(rooms.size() == 0)
                            {
                                rooms[roomIdCounter] = Room(roomIdCounter++);
                                cerr << "New room Created with roomID : " << (roomIdCounter - 1) << endl;
                            }

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

                            Packet toSend(S_OK);                                //Create a room and a response.
                            toSend.addArg(to_string(minRoomId));                //Add arguments to packet.
                            writePacket(toSend, fd);                            //Send.

                            break;
                        }

                        case OP_JOIN_RID:
                        {
                            if(request.getNumArgs != 1)
                                cerr << "ERROR: Not one argument." << endl;

                            int roomId = stoi(request.getArg(0));
                            if (rooms.find(roomId) == m.end() )                 //If the room doesnt exist.
                            {
                                writeStat(S_INVALID_ROOM_ID, fd);
                                break;
                            }
                                                                                //Join the player in that room.
                            Player newPlayer(user);
                            rooms[roomId].addPlayer(newPlayer);

                            Packet toSend(S_OK);                                //Join the room and send response.
                            writePacket(toSend, fd);                            //Send.

                            break;
                        }

                        case OP_HOST:
                        {
                            Packet toSend(S_OK);                                                     //Create a room and a response.
                            rooms[roomIdCounter] = Room(roomIdCounter);
                            toSend.addArg(to_string(roomIdCounter++));                               //Add arguments to packet.
                            writePacket(toSend, fd);                                                 //Send.
                            cerr << "New room Created with roomID : " << (roomIdCounter - 1) << endl;

                            break;
                        }

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

                        case OP_LEAVE:
                        {
                            break;
                        }

                        case OP_GET_WORD:
                        {
                            if(request.getNumArgs() == 1)
                            {
                                Packet toSend(S_OK);                                                     //Create a response
                                toSend.addArg(rooms[stoi(request.getArg(0))].getPlayerBoard());          //Add arguments to packet.
                                writePacket(toSend, fd);                                                 //Send.
                            }
                            else
                            {
                                writeStat(S_ERROR, fd);                                                  //Write an error if the args are invalid.
                            }

                            break;
                        }

                        case OP_GET_BOARD:
                        {
                            if(request.getNumArgs() == 1)
                            {
                                Packet toSend(S_OK);                                                     //Create a response
                                toSend.addArg(rooms[stoi(request.getArg(0))].getPlayerBoard());          //Add arguments to packet.
                                writePacket(toSend, fd);                                                 //Send.
                            }
                            else
                            {
                                writeStat(S_ERROR, fd);                                                  //Write an error if the args are invalid.
                            }

                            break;
                        }


                        case OP_UPDATE:
                            //pthread_mutex_lock(&mtx);
                            //pthread_mutex_unlock(&mtx);
                            break;

                        case OP_GET_ROOMS:
                        {
                            Packet toSend(S_OK);                                //Create a response

                            //pthread_mutex_lock(&mtx);
                            for(auto r: rooms)
                            {
                                toSend.addArg(to_string(r.second.getRoomId())); //Add arguments to packet.
                                //toSend.addArg(to_string(r.second.getNumPlayers()));
                            }

                            writePacket(toSend, fd);                            //Send.
                            //pthread_mutex_unlock(&mtx);
                            break;
                        }

                        case OP_GET_PLAYER_COUNT:
                        {
                            if(request.getNumArgs() == 1)
                            {
                                Packet toSend(S_OK);                                                      //Create a response
                                toSend.addArg(to_string(rooms[stoi(request.getArg(0))].getNumPlayers())); //Add arguments to packet.
                                writePacket(toSend, fd);                                                  //Send.
                            }
                            else
                            {
                                writeStat(S_ERROR, fd);                                                  //Write an error if the args are invalid.
                            }
                            break;
                        }

                        //If its an unIdentified Operation.
                        default:
                            cerr << "I get to unidentified opcode." << endl;
                            writeStat(S_ERROR, fd);
                            break;
                    }

                }

            }
            break;
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
