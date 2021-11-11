/* Name: Tommy Tran
 * UCID: 30086875
 * Class: CPSC 441 L01
 * 
 * server that uses UDP to provide a voting service to the client accessing it
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
#define PORT 8468
#define BUFFSIZE 4096

char* voting(char* request);
char* checkVote(char* num);
const char* view();
const char* votePrompt();
const char* tempView();

int globalkey = 5;
int votes1 = 123;
int votes2 = 234;
int votes3 = 345;
int votes4 = 17;
int listFlag = 0;
int voteFlag = 0;
int main(){
    cout << "voting started" << endl;
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
        response = voting(request);
        cout << "response is:" << response << endl;
        //sending message to client based on message received
        sendto(sockfd, response, BUFFSIZE, 0, (const struct sockaddr *) &clientAddr, len);
        cout << "response sent to client" << endl;

        //might have to add an exit or break if quit is called
    }  
}

//function that takes in the client request and returns the correct reply
char* voting(char* request){    
    const char* HELP = "HELP*";
    const char* HELPREPLY = "\n\rWE SUPPORT FOUR COMMANDS: 'SHOW*'(DISPLAYS A LIST OF CANDIDATES), 'VOTE*' (STARTS THE SECURE VOTING PROCESS),\n\r'SUMMARY*' (DISPLAYS THE CANDIDATES AND THEIR CURRENT VOTING COUNTS\n\rAND 'QUIT*'(QUITS THE SESSION)\n\r";
    const char* INVREQ = "\n\rINVALID REQUEST TRY 'SHOW*', 'VOTE*, 'SUMMARY*', OR 'QUIT*'\n\r";
    const char* QUIT = "QUIT*";
    const char* QUITREPLY = "#";
    const char* LIST = "SHOW*";
    const char* VIEW = "SUMMARY*";
    const char* VOTE = "VOTE*";
    const char* VIEWREPLY = tempView();
    const char* LISTREPLY = "\n\r1 - WILLIAM \n\r2 - BILLIAM \n\r3 - DILLIAM\n\r4 - HILLIAM\r\n";
    const char* VOTEREPLY1 = "\n\rYOU MUST USE 'SHOW*' FIRST\n\r";
    const char* ALREADYVOTED = "\n\rYOU HAVE ALREADY VOTED\n\r";
    const char* VIEWREPLY2 = "\n\rYOU HAVE NOT VOTED YET SO YOU DO NOT HAVE ACCESS TO THIS COMMAND\n\r";
    //cout << "temp is" << temp << endl;
    //const char* VOTEREPLY2 = votePrompt();
    //cout << "votereply2 is: " << VOTEREPLY2 << endl;
    if(strcmp(request, QUIT) == 0){
        return (char*) QUITREPLY;
    }
    if(voteFlag == 1){
        return checkVote(request);
    }
    if(strcmp(request, HELP) == 0){
        return (char*) HELPREPLY;
    } else if(strcmp(request, LIST) == 0){
        listFlag = 1;
        return (char*) LISTREPLY;
    } else if(strcmp(request, VOTE) == 0){
        if(listFlag == 0){
            return (char*) VOTEREPLY1;
        } else if (listFlag == 1){
            if(voteFlag == 2){
                return (char*) ALREADYVOTED;
            }
            voteFlag = 1;
            cout << "this is happening" << endl;
            return (char*) votePrompt();
        } else {
            return (char*) "SOMETHING BAD HAPPENED OH NO";
        }
    } else if(strcmp(request, VIEW) == 0){
        if(voteFlag == 2){
            return (char*) VIEWREPLY;
        } else {
            return (char*)VIEWREPLY2;
        }
    } else {
        return (char*) INVREQ;
    }
}

const char* votePrompt(){
    const char* help = "\n\rYOUR KEY IS 5\n\r";
    return help;
    // string temp = "\n\rYOUR KEY IS 5\n\r";
    // temp += to_string(globalkey);
    
    //  //temp.append("\n\r");
    //  cout << "voteprompt temp:" << temp << endl;
    //  cout << "cstring temp" << temp.c_str() << endl;
    // return temp.c_str();
    // int num = globalkey;
    // cout << to_string(num);
    // string key = to_string(num);
    // string temp = "YOUR KEY IS ";
    // temp += to_string(num);
    // //temp.append("\n\rVOTE FOR A CANDIDATE WITH THE FOLLOWING FORMAT \n\r'<CANDIDATE NUMBER>*' OR <CANDIDATE NUMBER*KEY>*'\n\r");
    // //cout << temp << endl;
    // //cout << temp.c_str() << endl;
    // return temp;
}

//function to check if the vote is valid or not
char* checkVote(char* num){
    const char* one = "1*";
    const char* two = "2*";
    const char* three = "3*";
    const char* four = "4*";
    string temp1 = to_string(globalkey);
    string temp2 = to_string(globalkey*2);
    string temp3 = to_string(globalkey*3);
    string temp4 = to_string(globalkey*4);
    temp1.append("*");
    temp2.append("*");
    temp3.append("*");
    temp4.append("*");
    const char* vote1 = "\n\rVOTE FOR WILLIAM COUNTED\n\r";
    const char* vote2 = "\n\rVOTE FOR BILLIAM COUNTED\n\r";
    const char* vote3 = "\n\rVOTE FOR DILLIAM COUNTED\n\r";
    const char* vote4 = "\n\rVOTE FOR HILLIAM COUNTED\n\r";
    const char* INVVOTE = "\n\rINVALID VOTE, PLEASE TRY AGAIN\n\r";
    if(strcmp(num, one) == 0 || strcmp(num, temp1.c_str()) == 0) {
        voteFlag++;
        votes1++;
        return (char*) vote1;
    } else if(strcmp(num, two) == 0 || strcmp(num, temp2.c_str()) == 0){
        votes2++;
        voteFlag++;
        return (char*) vote2;
    } else if(strcmp(num, three) == 0 || strcmp(num, temp3.c_str()) == 0){
        votes3++;
        voteFlag++;
        return (char*) vote3;
    } else if(strcmp(num, four) == 0 || strcmp(num, temp4.c_str()) == 0 ){
        votes4++;
        voteFlag++;
        return (char*) vote4;
    } else {
        return (char*) INVVOTE;
    }
}
//function to create the view string
const char* view(temp){
    string temp = "\n\rTHE VOTE COUNT IS CURRENTLY\r\n";
    string string1 = "WILLIAM - ";
    string num1 = to_string(votes1);
    string1.append(num1);
    string1.append("\n\r");
    //cout << string1 << endl;

    string string2 = "BILLIAM - ";
    string num2 = to_string(votes2);
    string2.append(num2);
    string2.append("\n\r");
    //cout << string2 << endl;

    string string3 = "DILLIAM - ";
    string num3 = to_string(votes3);
    string3.append(num3);
    string3.append("\n\r");
    //cout << string3 << endl;

    string string4 = "HILLIAM - ";
    string num4 = to_string(votes4);
    string4.append(num4);
    string4.append("\n\r");
    //cout << string4<< endl;
    
    temp.append(string1);
    temp.append(string2);
    temp.append(string3);
    temp.append(string4);

    //cout << temp << endl;
    //cout << temp.c_str();

    return temp.c_str();
}

const char* tempView(){
    return "\n\rWILLIAM - 123\n\rBILLIAM - 234\n\rDILLIAM - 345\n\rHILLIAM - 17";
}
