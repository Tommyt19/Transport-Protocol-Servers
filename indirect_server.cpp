/* Name: Tommy Tran
 * UCID: 30086875
 * Class: CPSC 441 L01
 * 
 * server that communicates in TCP with the client 
 * and sends requests to micro services via UDP
 * 
 * client will be using telnet
 * 
 * A lot of code was reused from my own previous assignment 1 submission proxy.cpp
 * mainly the implementation of the TCP server connection to the client
*/



/*
* TODO: FIGURE OUT THE SENDTO WITH TO ADDRESS AND TOLEN
* WHY IS THERE CLIENT AND IP_CLIENT
*/

// header filess
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

using namespace std;

#define PORT 8306
#define BUFFSIZE 2222
#define QUEUELEN 5
#define MONEYPORT 8466
#define FRENCHPORT 8467
#define VOTEPORT 8468

int findPort(char* num);
sockaddr_in serverInitialize(int serverPort);

int serverPort;
int quitFlag = 0;

int main(){

    //address initialization of the client address
    struct sockaddr_in clientAddr;
    memset(&clientAddr, 0, sizeof(clientAddr));
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(PORT);
    clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    //socket creation acting as server
    int clientSock = socket(AF_INET, SOCK_STREAM, 0);
    //socket creation error check
    if(clientSock == -1){
        cout << "creation of clientSock failed" << endl;
    }
    cout << "socket created" << endl ;
    //binding socket 
    int bindStatus;
    bindStatus = bind(clientSock, (struct sockaddr *) &clientAddr, sizeof(struct sockaddr_in));
    //error check for bind call
    if(bindStatus == -1){
        cout << ("bind call failed") << endl;
    }
    cout << ("bound") << endl;

    //listening time
    int listenStatus;
    listenStatus = listen(clientSock, QUEUELEN);
    //error checking listen call
    if(listenStatus == -1){
        cout << ("listen call failed") << endl;
    }
    cout << "listening hehe" << endl;
    /*
    *Loop for server to listen to requests
    */
   //initialization of process id to keep track of forks
   int pid;
   //loop for server to listen to requests
   while(1){
        cout << "entered while loop" << endl;
        //connection acceptance
        int acceptedSock;
        acceptedSock = accept(clientSock, NULL, NULL);
        //error checking accept call
        if(acceptedSock == -1){
            cout << ("accept call failed") << endl;
        }
        cout << ("socket accepted") << endl;
        //creating of child to deal with new client 
        pid = fork();
        //error check if fork call fails to create child
        if(pid < 0){
            cout << ("forking failed, try spoon next?") << endl;
        } else if (pid == 0){
            // child process is inside of this code block
            //no longer need inherited parent socket?
            close(clientSock);
            //receiving message from the client
            int clientCount;
            const char* activateKey = "*";
            char* activeMsg = new char;
            //receive request from server
            while(1){
                clientCount = recv(acceptedSock, activeMsg, 1, 0);
                //error checking receiving message from client
                if(clientCount == -1){
                    cout << ("receiving message from client failed") << endl;
                }
                cout << "current activeMsg is:" << activeMsg << endl;
                if(strcmp(activeMsg, activateKey) == 0){
                    break;
                }
            }
            cout << "ACTIVATED" << endl;

            //display message
            const char* displayList = "\r1 - TRANSLATOR \n\r2 - CURRENCY CONVERTER \n\r3 - VOTING\n\r4 - QUIT SESSION\n\r";

restartLoop:
            clientCount = send(acceptedSock, displayList, strlen(displayList), 0);
            if(clientCount == -1){
                cout << "sending displaylist failed" << endl;
            }
            cout << "sent server list" << endl;

            int key;
            const char* wrongKey = "\n\rYOU FAILED TO SELECT A CORRECT SERVICE TRY AGAIN\n\r";
            //receiving service choice loop
            while(1){
                clientCount = recv(acceptedSock, activeMsg, 1, 0);
                if(clientCount == -1){
                    cout << "receiving server choice failed" << endl;
                }
                key = findPort(activeMsg);
                if(key == 1){
                    serverPort = FRENCHPORT;
                    break;
                } else if (key == 2) {
                    serverPort = MONEYPORT;
                    break;
                } else if (key == 3) {
                    serverPort = VOTEPORT;
                    break;
                } else if (key == 4){
                    quitFlag = 1;
                    break;
                } else {
                    clientCount = send(acceptedSock, wrongKey, strlen(wrongKey), 0);
                }
            }
            if(quitFlag == 1){
                break;
            }
            cout << "Port chosen was:" << serverPort << endl;
            /*
            * implement connection to udp microservice
            */
            struct sockaddr_in serverAddr = serverInitialize(serverPort);
            //socket creation as client
            int serverSock = socket(AF_INET, SOCK_DGRAM, 0);
            //socket creation error check
            if(serverSock == -1){
                cout << ("creation of serverSock failed") << endl;
            }

            //setting a timeout for the socket by setting a timeout option for the socket
            struct timeval time;
            time.tv_sec = 5;
            time.tv_usec = 17;

            int timeCheck;
            timeCheck = setsockopt(serverSock, SOL_SOCKET, SO_RCVTIMEO, &time, sizeof (time));
            if(timeCheck == -1){
                cout << "setting rcv timeout failed" << endl;
            }
            timeCheck = setsockopt(serverSock, SOL_SOCKET, SO_SNDTIMEO, &time, sizeof (time));
            if(timeCheck == -1){
                cout << "setting snd timeout failed" << endl;
            }
            
            //initializing serverCount and len
            int serverCount;
            int len;
            const char* requestPrompt = "\n\rSERVICE SELECTED INSERT REQUEST, IF UNSURE JUST SEND 'HELP', END ALL REQUESTS WITH A '*' CHARACTER\n\r";
            clientCount = send(acceptedSock, requestPrompt, strlen(requestPrompt), 0);
            if(clientCount == -1){
                cout << "sending request prompt failed" << endl;
            }

            //quit key and msg for ending server interaction
            const char* quitKey = "#";
            const char* quitMsg = "\n\rAWW NO MORE FUN WITH THE MICROSERVICE?\n\rT_T HAVE A GOOD DAY\n\r";
            //loop for interacting with request
            while(1){
                //counter for number of chars
                int i = 0;
                char* clientMsg = new char[BUFFSIZE];
                memset(clientMsg, 0, BUFFSIZE);
                while(1){
                    clientCount = recv(acceptedSock, activeMsg, 1, 0);
                    if(clientCount == -1){
                        cout << "receiving request failed" << endl;
                    }
                    strcat(clientMsg, activeMsg);
                    i++;
                    if(strcmp(activeMsg, activateKey) == 0){
                        clientMsg[i] = '\0';
                        break;
                    }
                }
                //sending the request to the microservice
                cout << "clientMsg is:" << clientMsg << endl;
                sendto(serverSock, clientMsg, strlen(clientMsg), MSG_CONFIRM, (const struct sockaddr *) &serverAddr, sizeof(serverAddr));
                cout << "sent msg to server" << endl;
                //receiving message from server
                char* reply = new char[BUFFSIZE];
                serverCount = recvfrom(serverSock, reply, BUFFSIZE, MSG_WAITALL, (struct sockaddr *) &serverAddr, (socklen_t *) &len);
                if(strcmp(reply, quitKey) == 0){
                    send(acceptedSock, quitMsg, strlen(quitMsg), 0);
                    if(clientCount == -1){
                        cout << "sending quitmsg failed" << endl;
                    }
                    goto restartLoop;
                }
                reply[serverCount] = '\0';
                clientCount = send(acceptedSock, reply, strlen(reply), 0);
                //sending reply back to client
                cout << "sent message back to client" << endl;
                //error checking sending message back to client
                if(clientCount == -1){
                cout << ("sending message to client failed") << endl;
                }
            }
            //closing socket and exit the loop
            cout << "closing serverSock" << endl;
            close(serverSock);
            exit(0);

        } else {
            //parent process once again
            cout << ("the child has been created and given resposibilities hyuck!") << endl;
            cout << ("parent listening once again") << endl;

            //parent does not need acceptedsocket
            close(acceptedSock);
        }

    }
    //closing sockets 
    close(clientSock);
    return 0;
}

int findPort(char* num){
    //SERVER KEYS
    const char* one = "1";
    const char* two = "2";
    const char* three = "3";
    const char* four = "4";
    if(strcmp(num, one) == 0){
        return 1;
    } else if (strcmp(num, two) == 0){
        return 2;
    } else if (strcmp(num, three) == 0){
        return 3;
    } else if (strcmp(num, four) == 0){
        return 4;
    } else {
        return 5;
    }
}

sockaddr_in serverInitialize(int serverPort){
    //Address initialization of the server address
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    serverAddr.sin_addr.s_addr = inet_addr("136.159.5.25");
    cout << "address initialization attempted" << endl;
    return serverAddr;
}



