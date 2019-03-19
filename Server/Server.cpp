#include <QApplication>
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
#include <regex>          // Regular Expression
#include <pthread.h>      // pthread
#include <stdexcept>      //Excpetions
#include <sys/time.h>     // time
#include <vector>

#define OP_CREATE_ACT 1
#define OP_LOGIN 2
#define OP_JOIN 3
#define OP_JOIN_RID 4
#define OP_HOST 5
#define OP_INVITE 6
#define OP_GET_PLAYERS 7
#define OP_WON 8
#define OP_LOST 9
#define OP_GET_WORD 10
#define OP_GET_BOARD 11
#define OP_UPDATE 12
#define OP_GET_ROOMS 13
#define OP_GET_PLAYER_COUNT 14
#define OP_UNREGISTER 15

//Status codes (Only one byte).
#define S_OK 1
#define S_ERROR 2
#define S_NOT_IN_ROOM 3
#define S_ROOM_FULL 4
#define S_INVALID_ROOM_ID 5
#define S_AUTH_ERROR 6
#define S_INVALID_USER_PASS 7
#define S_GAME_OVER 8
#define S_NOT_LOGGED_IN 9

using namespace std;

class Server {
private:
    int port;

    int playerCount;
    vector<Room> rooms;
    Player players; //do I need this?
    User user; // didn't use this
    
    string [] wordArray = new string ["hello", "word"];         //Array of words
    vector<int> roomIDVector{};                                 //rooms taken

public:
    Server(int serverPort) {
        // Reject invalid ports
//        if((serverPort > 65535) || (serverPort < 1024)) {
//            perror("Please enter a port number between 1024 - 65535");
        if((serverPort != 59123)) {
            perror("Please enter a port number 59123"); // I just set this randomly
        }
        port = serverPort;
    }

    void startServer() {
        pthread_t thread1;
        // Declare a sockaddr_in structure
        sockaddr_in acceptSockAddr;
        
        // zero-initialize it by calling bzero 
        bzero((char*) &acceptSockAddr, sizeof(acceptSockAddr));
        
        // and set its data members
        acceptSockAddr.sin_family       = AF_INET; // Address Family Internet
        acceptSockAddr.sin_addr.s_addr  = htonl( INADDR_ANY );
        acceptSockAddr.sin_port         = htons( port );
        

        //create socket
        int serverSd = -1;
        serverSd = socket( AF_INET, SOCK_STREAM, 0 );
        
        // socket is opened
        if(serverSd < 0) {
            perror("Cannot open socket");
        }

        // Set the SO_REUSEADDR option. (Note this option is useful to prompt OS to release the server port as soon as your server process is terminated.)
        const int on = 1;
        setsockopt( serverSd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, 
                    sizeof( int ) );

        // Bind this socket to its local address by calling bind as passing the following arguments: the socket descriptor, 
        // the sockaddr_in structure defined above, and its data size. 
        bind(serverSd, (sockaddr * ) & acceptSockAddr, sizeof(acceptSockAddr));

        //Looping for new clients attemping to connect
        while (true) {
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
    
    // TODO
    static void *threadRequestHandler(void *vptr_value) {
        int *arr = (int *) vptr_value;
        
        char opCode = readOP();
        string username;
        string password;
        string word;
        
        if(opCode.equals(OP_CREATE_ACT)) {
            string userInput = readString();
            string passInput = readString();

            int status = S_ERROR;
            if (user == NULL || pass == NULL) {
                status = S_INVALID_USER_PASS;
            } else {
                username = userInput;
                password = passInput;
                status = S_OK;
            }
            writeInt(status);
        } else if (opCode.equals(OP_LOGIN)) {
            string userInput = readString();
            string passInput = readString();
            
            int status = S_ERROR;
            if (!password.equals(passInput) && userInput.equals(username)) {
                status = S_AUTH_ERROR;
            }
            if (userInput == NULL || !userInput.equals(username) ||
                passInput == NULL || !passInput.equals(password)) {// user is invalid or password is invalid
                status = S_INVALID_USER_PASS;
            }
            if (username.equals(user) && password.equals(pass)) {// user is valid and password is valid User.password.equals(password)?
                status = S_OK;
            }
            writeInt(status);
            
        } else if (opCode.equals(OP_JOIN)) {
            int id = -1;
            int status = S_ERROR;
            if (username != NULL && password != NULL) {// not logged in
                status = S_NOT_LOGGED_IN;
            } else {
                for (int i = 0; i < rooms.end(); i++) {
                    if (rooms.at(i).getNumPlayers() < 2) {  // roomID has space
                        id = i;
                        // add player username to room????????????????????????????
                        status = S_OK;
                        break;
                    }
                }
                if (id == -1) {
                    id = rooms.end()+1;
                    rooms.push_back();
                }
            }
            writeInt(id);
            writeInt(status);
        } else if (opCode.equals(OP_JOIN_RID)) {
            int status = S_ERROR;
            if (roomID == NULL) {// roomID invalid
                status = S_INVALID_ROOM_ID;
            }
            for (int i = 0; i < rooms.end(); i++) {
                if (rooms.at(i).getRoomID() == roomID &&
                    rooms.at(i).getNumPlayers() < 2) {  // roomID is valid
                    status = S_OK;
                    break;
                } else if (rooms.at(i).getRoomID() == roomID &&
                           rooms.at(i).getNumPlayers() == 2) { // roomID is full
                    status = S_ROOM_FULL;
                    break;
                } else {
                    status = S_INVALID_ROOM_ID;
                }
            }
            writeInt(status);
        } else if (opCode.equals(OP_HOST)) {
            int status = S_ERROR;
            if (username != NULL && password != NULL) {// not logged in
                status = S_NOT_LOGGED_IN;
            } else {
                Room newRoom = new Room;
                if (rooms.push_back(Room)) {// room created?
                    status = S_OK;
                }
            }
            writeInt(status);
            writeInt(newRoom.getRoomID());
        } else if (opCode.equals(OP_INVITE)) {
            int roomId = readInt();
            string userInput = readString();
            int status = S_ERROR;
            if (username != NULL && password != NULL) {// not logged in
                status = S_NOT_LOGGED_IN;
            } else {
                for (int i = 0; i < rooms.end(); i++) {
                    if (rooms.at(i).getRoomID() == roomId &&
                        rooms.at(i).getNumPlayers() == 1 &&
                        rooms.at(i).players.at(0) == userInput) {  // roomID is valid CHECK THE PLAYER OBJECT, MAYBE NEEDS TO BE A STRING ARRAY???
                        status = S_OK;
                        break;
                    } else if (rooms.at(i).getRoomID() == roomId &&
                               rooms.at(i).getNumPlayers() == 1 &&
                               rooms.at(i).players.at(0) != userInput) {
                        status = S_INVALID_ROOM_ID;
                        break;
                    } else if (rooms.at(i).getRoomID() == roomId &&
                               rooms.at(i).getNumPlayers() == 2) { // roomID is full
                        status = S_ROOM_FULL;
                        break;
                    } else {
                        status = S_INVALID_ROOM_ID;
                    }
                }
            }
            writeInt(status);
        } else if (opCode.equals(OP_GET_PLAYERS)) {
            int roomId = readInt();
            int status = S_ERROR;
            int index = 0;
            if (username != NULL && password != NULL) {// not logged in
                status = S_NOT_LOGGED_IN;
            } else {
                for (int i = 0; i < rooms.end(); i++) {
                    if (rooms.at(i).getRoomID() == roomId) {  // roomID is valid
                        index = i;
                        status = S_OK;
                        break;
                    } else {
                        status = S_INVALID_ROOM_ID; // invalid roomID
                    }
                }
            }
            writeInt(status);
            writeInt(rooms.at(index).getPlayers());
        } else if (opCode.equals(OP_WON)) {
            int roomId = readInt();
            int status = S_ERROR;
            int index = 0;
            if (username != NULL && password != NULL) {// not logged in
                status = S_NOT_LOGGED_IN;
            } else {
                for (int i = 0; i < rooms.end(); i++) {
                    if (rooms.at(i).getRoomID() == roomId) {  // roomID is valid
                        index = i;
                        status = S_OK;
                        break;
                    } else {
                        status = S_INVALID_ROOM_ID; // invalid roomID
                    }
                }
                
                if () {// invalid room ID rooms.at(index) // HOW TO END GAME??????
                    status = S_GAME_OVER;
                }
            }
            writeInt(status);
        } else if (opCode.equals(OP_LOST)) {
            int roomId = readInt();
            int status = S_ERROR;
            int index = 0;
            if (username != NULL && password != NULL) {// not logged in
                status = S_NOT_LOGGED_IN;
            } else {
                for (int i = 0; i < rooms.end(); i++) {
                    if (rooms.at(i).getRoomID() == roomId) {  // roomID is valid
                        index = i;
                        status = S_OK;
                        break;
                    } else {
                        status = S_INVALID_ROOM_ID; // invalid roomID
                    }
                }
                
                if () {// invalid room ID rooms.at(index) // HOW TO END GAME??????
                    status = S_GAME_OVER;
                }
            }
            writeInt(status);
        } else if (opCode.equals(OP_GET_WORD)) {
            int roomId = readInt();
            int status = S_ERROR;
            int index = 0;
            word = "placeholder";
            if (username != NULL && password != NULL) {// not logged in
                status = S_NOT_LOGGED_IN;
            } else {
                for (int i = 0; i < rooms.end(); i++) {
                    if (rooms.at(i).getRoomID() == roomId) {  // roomID is valid
                        index = i;
                        status = S_OK;
                        break;
                    } else {
                        status = S_INVALID_ROOM_ID; // invalid roomID
                    }
                }
                word = rooms.at(index).generateWord();
            }
            writeInt(status);
            writeString(word);
        } else if (opCode.equals(OP_GET_BOARD)) {
            int roomId = readInt();
            int status = S_ERROR;
            string playerBoard = "placeholder";
            if (username != NULL && password != NULL) {// not logged in
                status = S_NOT_LOGGED_IN;
            } else {
                if () {                              // What do I check & update???
                    status = S_OK;
                }
            }
            writeInt(status);
            writeString(playerBoard);
        } else if (opCode.equals(OP_UPDATE)) {
//            update()
            int roomId = readInt();
            int status = S_ERROR;
            if (username != NULL && password != NULL) {// not logged in
                status = S_NOT_LOGGED_IN;
            } else {
                for (int i = 0; i < rooms.end(); i++) {
                    if (rooms.at(i).getRoomID() == roomId) {  // roomID is valid
                        index = i;                  // UPDATE PLAYER HOW?????
                        status = S_OK;
                        break;
                    } else {
                        status = S_INVALID_ROOM_ID; // invalid roomID
                    }
                }
            }
            writeInt(status);
        } else if (opCode.equals(OP_GET_ROOMS)) {
            int status = S_ERROR;
            if (rooms.end() != NULL) {
                status = S_OK;
                writeInt(status);
                writeInt(rooms.end());
                for (int i = 0; i < rooms.end(); i++) {
                    writeInt(rooms.at(i).getRoomID());
                }
            } else {
                writeInt(status);
            }
        } else if (opCode.equals(OP_GET_PLAYER_COUNT)) {
            int roomId = readInt();
            int status = S_ERROR;
            int count = 0;
            for (int i = 0; i < rooms.end(); i++) {
                if (rooms.at(i).getRoomID() == roomId) {  // roomID is valid
                    count = rooms.at(i).players.size();
                    status = S_OK;
                    break;
                } else {
                    status = S_INVALID_ROOM_ID; // invalid roomID
                }
            }
            writeInt(status);
            writeInt(count);
        } else if (opCode.equals(OP_UNREGISTER)) {            // HOW???????
            int status = S_ERROR;
            if () {
                status = S_OK;
            }
            writeInt(status);
        } else {
            request = S_ERROR;
        }

        //close socket                              // when do I do this???
        shutdown(arr[0], SHUT_RDWR);
        close(arr[0]);
    }


};

int readOP()
{
    char status;
    read(fd, &status, sizeof(status));
    
    return static_cast<int>(status);
}

string readString() {
    int strSize = 0;
    read(fd, &strSize, sizeof(strSize));
    
    char* buffer = new char[strSize];
    read(fd, &buffer, sizeof(buffer));
    
    string toReturn(buffer);
    delete[] buffer;
    
    return toReturn;
}

void Connection::writeString(string toWrite)
{
    int strSize = static_cast<char>(toWrite.size());
    ::write(fd, &strSize, sizeof(strSize));
    ::write(fd, toWrite.c_str(), toWrite.size());
}

void writeInt(int toWrite) {
    ::write(fd, &toWrite, sizeof(toWrite));
}

int readInt() {
    int toReturn = 0;
    read(fd, &toReturn, sizeof(toReturn));
    
    return toReturn;
}

int main(int argc, char *argv[]) {
    if (argc > 2) {
        perror("Invalid argument input");
        return -1;
    } else {
        Server(atoi(argv[1])).startServer();
    }
    return 1;
}
