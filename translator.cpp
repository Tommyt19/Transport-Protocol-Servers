/* Name: Tommy Tran
 * UCID: 30086875
 * Class: CPSC 441 L01
 * 
 * server that uses UDP to provide a small translation service to the client accessing it
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
#define PORT 8467
#define BUFFSIZE 4096

char* translate(char* request);

int main(){
    cout << "translator started" << endl;
    //socket file descriptor
    int sockfd;
    //buffer for sending back messages
    char *response;

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
    while(1){
        char* request = new char[BUFFSIZE];
        //receiving message from client
        recvfrom(sockfd, (char *) request, BUFFSIZE, 0, (struct sockaddr *) &clientAddr, (socklen_t*) &len);
        //printing out message received
        cout << request << endl;
        response = translate(request);
        //sending message to client based on message received
        sendto(sockfd, (const char*) response, strlen(response), 0, (const struct sockaddr *) &clientAddr, len);
        cout << "response sent to client" << endl;

        //might have to add an exit or break if quit is called
    }
   
}
char* translate(char* request){
    const char* ENGLISH1 = "HELLO*";
    const char* ENGLISH2 = "YES*";
    const char* ENGLISH3 = "NO*";
    const char* ENGLISH4 = "ONE*";
    const char* ENGLISH5 = "TWO*";
    const char* HELP = "HELP*";
    const char* HELPREPLY = "\n\rSIMPLY SEND ONE OF THE FOLLOWING WORDS IN ALL CAPS WITH A '*' AT THE END: 'HELLO', 'YES', 'NO', 'ONE' OR 'TWO'\n\r";
    const char* QUIT = "QUIT*";
    const char* QUITREPLY = "#";
    const char* FRENCH1 = "\n\rFRENCH: BONJOUR\n\r";
    const char* FRENCH2 = "\n\rFRENCH: OUI\n\r";
    const char* FRENCH3 = "\n\rFRENCH: NON\n\r";
    const char* FRENCH4 = "\n\rFRENCH: UN\n\r";
    const char* FRENCH5 = "\n\rFRENCH: DEUX\n\r";
    const char* INVREQ = "\n\rINVALID REQUEST TRY ALL CAPS NEXT TIME, THE FOLLOWING WORDS ARE THE ONLY ONES THAT WORK:'HELLO', 'YES', 'NO', 'ONE', AND 'TWO'\n\r";
    if(strcmp(request, ENGLISH1) == 0){
        return (char *)FRENCH1;
    } else if(strcmp(request, ENGLISH2) == 0){
        return (char *)FRENCH2;
    } else if(strcmp(request, ENGLISH3) == 0){
        return (char *)FRENCH3;
    } else if(strcmp(request, ENGLISH4) == 0){
        return (char *)FRENCH4;
    } else if (strcmp(request, ENGLISH5) == 0){
        return (char *)FRENCH5;
    } else if (strcmp(request, HELP) == 0){
        return (char*)HELPREPLY;
    } else if (strcmp(request, QUIT) == 0){
        return (char*)QUITREPLY;
    } else {
        return (char*)INVREQ;
    }
}