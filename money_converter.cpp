/* Name: Tommy Tran
 * UCID: 30086875
 * Class: CPSC 441 L01
 * 
 * server that uses UDP to provide a money conversion service to the client accessing it
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
#define PORT 8466
#define BUFFSIZE 4096
#define USCAD 1.24
#define BTCCAD 77572
#define EUROCAD 1.44
#define POUNDCAD 1.70
#define CADCAD 1.00

char* convert(char* request);
double findRate(char token[]);

int main(){
    cout << "converter started" << endl;
    //char test[]= "$10 CAD USD";
    //convert(test);

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
        response = convert(request);
        //sending message to client based on message received
        sendto(sockfd, (const char*) response, strlen(response), 0, (const struct sockaddr *) &clientAddr, len);
        cout << "response sent to client" << endl;

        //might have to add an exit or break if quit is called
    }  
}

char* convert(char request[]){
    const char* HELP = "HELP*";
    const char* HELPREPLY = "\n\rSIMPLY SEND A REQUEST IN THE FOLLOWING FORMAT IN ALL CAPS:\n\r'$<NUMBER VALUE> <CURRENCY> <CURRENCY>*'\n\rWE SUPPORT THE FOLLOWING CURRENCY CODES: CAD, USD, BTC, EUR, GBP\n\r";
    const char* INVREQ = "\n\rINVALID REQUEST TRY TO SEND A REQUEST IN THE FOLLOWING FORMAT:\n\r'$<NUMBER VALUE> <CURRENCY> <CURRENCY>*'\n\rWE SUPPORT THE FOLLOWING CURRENCY CODES: CAD, USD, BTC, EUR, GBP\n\r";
    const char* QUIT = "QUIT*";
    const char* QUITREPLY = "#";
    if(strcmp(request, QUIT) == 0){
        return (char*)QUITREPLY;
    }
    if(strcmp(request, HELP) == 0){
        return (char *) HELPREPLY;
    }
    char* numVal = strtok(request, " ");
    if(strchr(numVal, '$') == nullptr){
        return (char*)INVREQ;
    }

    char* curr1 = strtok(NULL, " ");
    char* curr2 = strtok(NULL, "*");

    string temp = numVal;
    temp.erase(0,1);
    double initVal = strtod(temp.c_str(), nullptr);

    double rate1 = findRate(curr1);
    double rate2 = findRate(curr2);
    if((rate1 == 0.357) || (rate2 == 0.357)){
        return (char*) INVREQ;
    }
    double finalVal = initVal*findRate(curr1)/findRate(curr2);
    temp = to_string(finalVal);
    string dollarTemp = "$";
    dollarTemp.append(temp);

    string finalTemp = "\n\r";
    finalTemp.append( dollarTemp.substr(0, dollarTemp.find(".") + 3));
    finalTemp.append("\n\r");
    char* returnString = (char*)finalTemp.c_str();
    
    cout << "return string is:" << returnString << endl;
    return returnString;
}

double findRate(char token[]){
    const char* USD = "USD";
    const char* CAD = "CAD";
    const char* BTC = "BTC";
    const char* GBP = "GBP";
    const char* EUR = "EUR";
    double error = 0.357;
    if(strcmp(token, USD) == 0){
        return USCAD;
    } else if(strcmp(token, CAD) == 0){
        return CADCAD;
    } else if (strcmp(token, EUR) == 0){
        return EUROCAD;
    } else if (strcmp(token, BTC) == 0){
        return BTCCAD;
    } else if (strcmp(token, GBP) == 0){
        return POUNDCAD;
    } else {
        return error;
    }
}
