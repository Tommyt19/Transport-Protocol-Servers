/* Name: Tommy Tran
 * UCID: 30086875
 * Class: CPSC 441 L01
 * 
 * server that uses UDP to provide a micro-service to the client accessing it
 * 
 * Uses TA Soroosh Esmaeillian's UDP_server code for basic structure and implementation of a UDP server
 * https://d2l.ucalgary.ca/d2l/le/content/401094/viewContent/4936270/View
 */

// including libraries
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

using namespace std;

//definitions for port and buffer size
#define PORT 8465
#define BUFFSIZE 4096

int main(){
    //socket file descriptor
    int sockfd;
    //buffer for receiving messages
    char request[BUFFSIZE];
    //buffer for sending back messages
    char *response = "bonjour le message est sent";

    //address initialization of server address
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(PORT);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    //create socket 
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("socket creation did not work sad");
        exit(EXIT_FAILURE);
    }
    //binding socket to server address
    if (bind(sockfd, (const struct sockaddr *)&servAddr, sizeof(servAddr)) < 0){
        perror("bind call failed sadder");
        exit(EXIT_FAILURE);
    }

    //clientAddr for receiving and sending to
    struct sockaddr_in clientAddr;
    memset(&clientAddr, 0, sizeof(clientAddr));

    //length is size of clientAddr
    int len = sizeof(clientAddr);
    //recvCount is count of bytes that is received from recvfrom call
    int recvCount;

    //receiving message from client
    recvCount = recvfrom(sockfd, (char *) request, BUFFSIZE, 0, (struct sockaddr *) &clientAddr, (socklen_t*) &len);
    //printing out message received
    cout << request << endl;
    //sending message to client based on message received
    sendto(sockfd, (const char*) response, strlen(response), 0, (const struct sockaddr *) &clientAddr, len);
    cout << "response sent to client" << endl;

}

