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

using namespace std;

class Server {
private:
    int port;

    int playerCount;
    vector<Room> rooms;
    Player players;
    User user;
    
    string [] wordArray = new string ["hello", "word"];         //Array of words
    vector<int> roomIDVector{};                                 //rooms taken

	int ok = 1;
	int error = 2;
	int notInRoom = 3;
	int roomFull = 4;
	int invalidRoomID = 5;
	int authError = 6;
	int invalidUserPass = 7;
	int gameOver = 8;
	int notLoggedIn = 9;

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

    int create_account(User userObj, int pubKey) {
    	int status = error;
        if (userObj == NULL || userObj.publicKey != pubKey) {
            status = invalidUserPass;
        }
    	if (userObj.publicKey == pubKey) {// user is valid and public key is valid
    		status = ok;
    	}
    	return status;
    }

    int login(User userObj, string password) {
    	int status = error;
    	if () {// user is invalid or password is invalid
    		status = invalidUserPass;
    	}
    	if () {
    		status = authError;
    	}
    	if () {// user is valid and password is valid User.password.equals(password)?
    		status = ok;
    	}
    	return status;
    }

    // returns multiple status codes: status + roomID
    int join() {
    	int status = error;
    	if () {// roomID invalid
    		status = invalidRoomID;
    	}
    	if () {// roomID is valid
    		status = ok;
    	}
    	return status + roomID;
    }

    int join(int roomID) {
    	int status = error;
    	if (roomID == NULL) {// roomID invalid
    		status = invalidRoomID;
    	}
        for (int i = 0; i < rooms.end(); i++) {
            if (rooms.at(i).getRoomID() == roomID &&
                rooms.at(i).getNumPlayers() < 2) {  // roomID is valid
                status = ok;
                break;
            } else if (rooms.at(i).getRoomID() == roomID &&
                       rooms.at(i).getNumPlayers() == 2) { // roomID is full
                status = roomFull;
                break;
            } else {
                status = invalidRoomID;
            }
        }
    	return status;
    }

    // returns multiple status codes: status + roomID
    int host() {
    	int status = error;
        Room newRoom = new Room;
    	if (rooms.push_back(Room)) {// room created?
    		status = ok;
        }
    	return status + newRoom.getRoomID();
    }

    int invite(int roomID, string username) {
    	int status = error;
        if () {// invalid user/pass
            status = invalidUserPass;
        }
        if () {// not logged in
            status = notLoggedIn;
        }
        if () {// not in room
            status = notInRoom;
        }
        for (int i = 0; i < rooms.end(); i++) {
            if (rooms.at(i).getRoomID() == roomID &&
                rooms.at(i).getNumPlayers() < 2) {  // roomID is valid
                status = ok;
                break;
            } else if (rooms.at(i).getRoomID() == roomID &&
                       rooms.at(i).getNumPlayers() == 2) { // roomID is full
                status = roomFull;
                break;
            } else {
                status = invalidRoomID; // invalid roomID
            }
        }
    	return status;
    }

    // returns status + Player[]
    Player[] getPlayers(int roomID) {
    	int status = error;
        int index = 0;
        for (int i = 0; i < rooms.end(); i++) {
            if (rooms.at(i).getRoomID() == roomID) {  // roomID is valid
                index = i;
                status = ok;
                break;
            } else {
                status = invalidRoomID; // invalid roomID
            }
        }
        return status; // status + Player[]? rooms.at(index)?
    }

    int won(int roomID) {
    	int status = error;
        int index = 0;
        for (int i = 0; i < rooms.end(); i++) {
            if (rooms.at(i).getRoomID() == roomID) {  // roomID is valid
                index = i;
                status = ok;
                break;
            } else {
                status = invalidRoomID; // invalid roomID
            }
        }

    	if () {// invalid room ID rooms.at(index)
    		status = gameOver;
    	}
    	return status;
    }

    int lost(int roomID) {
        int status = error;
        int index = 0;
        for (int i = 0; i < rooms.end(); i++) {
            if (rooms.at(i).getRoomID() == roomID) {  // roomID is valid
                index = i;
                status = ok;
                break;
            } else {
                status = invalidRoomID; // invalid roomID
            }
        }
        
        if () {// invalid room ID rooms.at(index)
            status = gameOver;
        }
        return status;
    }

	// return status+word!
    int getWord(int roomID) {
    	int status = error;
        int status = error;
        int index = 0;
        for (int i = 0; i < rooms.end(); i++) {
            if (rooms.at(i).getRoomID() == roomID) {  // roomID is valid
                index = i;
                status = ok;
                break;
            } else {
                status = invalidRoomID; // invalid roomID
            }
        }
    	return status; // status + word from? rooms.at(index).getWord();?
    }

	// return status+players array
    int getPlayerBoard(int roomID) {
    	int status = error;
    	if () {
    		status = ok;
    	}
    	return status + players[]; // players array?
    }

    int update(int roomID, Player player) {
    	int status = error;
    	if () {
    		status = invalidRoomID;
    	}
    	if () {
    		status = ok;
    	}
    	return status;
    }

    // return status + roomID[]
    int getRoom() {
    	int status = error;
    	if () {
    		status = ok;
    	}
    	return status + roomID; //roomID[]?
    }

    // return status + playerCount
    int getPlayerCount(int roomID) {
    	int status = error;
    	if () {
    		status = ok;
    	}
    	return status + playerCount;
    }

    int unregister() {
    	int status = error;
    	if () {
    		status = ok;
    	}
    	return status;
    }
    static void *threadRequestHandler(void *vptr_value) {
         int *arr = (int *) vptr_value;
         char dataBuff[4000];
         void *retVal = malloc(sizeof(int));


         int nRead = 0;
         string request = "";
         //Receive Request
         int bytesRead = recv(arr[0], dataBuff, 4000 - nRead, 0);

        // //Nothing in GET Request
         if (bytesRead <= 0) {
             shutdown(arr[0], SHUT_RDWR);
             close(arr[0]);
             return retVal;
         }


         nRead += bytesRead;
         request.append(dataBuff, bytesRead);
        
        if(dataBuff.equals("1")) {
//            create_account()
        } else if (dataBuff.equals("2")) {
//            login()
        } else if (dataBuff.equals("3")) {
//            join()
        } else if (dataBuff.equals("4")) {
//            join()
        } else if (dataBuff.equals("5")) {
//            host()
        } else if (dataBuff.equals("6")) {
//            invite()
        } else if (dataBuff.equals("7")) {
//            getPlayers()
        } else if (dataBuff.equals("8")) {
//            won()
        } else if (dataBuff.equals("9")) {
//            lost()
        } else if (dataBuff.equals("10")) {
//            getWord()
        } else if (dataBuff.equals("11")) {
//            getPlayerBoard()
        } else if (dataBuff.equals("12")) {
//            update()
        } else if (dataBuff.equals("13")) {
//            getRoom()
        } else if (dataBuff.equals("14")) {
//            getPlayerCount()
        } else if (dataBuff.equals("15")) {
//            unregister()
        } else {
            request = error;
        }

        //close socket
        shutdown(arr[0], SHUT_RDWR);
        close(arr[0]);
    }


};

int main(int argc, char *argv[]) {
    if (argc > 2) {
        perror("Invalid argument input");
        return -1;
    } else {
        Server(atoi(argv[1])).startServer();
    }
    return 1;
}
