/*******************************************************************************
    Winnie Yang
    Computer Project #11
*******************************************************************************/

#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#define max 64      // buffer is 64 bytes max

using namespace std;

/*---------------------------------------------------------------------------
    Name:  main
    Purpose: Do all the work for this project
    Receive: Number of arguments int, arguments in char*
    Return: None
---------------------------------------------------------------------------*/
int main(int argc, char* argv[]) 
{
    // Checking if exactly 4 arguments are entered
    if (argc != 4)
    {
        cout << "Error: Need exact 4 arguments in exact order -> client, host, port, and file." << endl;
        exit(1);
    }

    // Checking if host name is valid
    char *host = argv[1];       // host: arctic.cse.msu.edu
    struct hostent *server = gethostbyname(host);
    if (server == NULL)
    {
        cout << "Error: Host -> " << host << "<- does not exist." << endl;
        exit(2);
    }

    // Creating socket
    int client = socket(AF_INET, SOCK_STREAM, 0);  
    if (client < 0)
    {
        cout << "Error: Cannot create socket." << endl;
        exit(3);
    }

    // Creating socket structure
    struct sockaddr_in socketAddress; 
    bzero(&socketAddress, sizeof(socketAddress));   // configure server address
    socketAddress.sin_family = AF_INET;     // TCP/IP
    unsigned int port = atoi(argv[2]);      // port number
    socketAddress.sin_port = htons(port);   // port provided for connection
    // copy n bytes from server to socket address
    bcopy(server->h_addr, &socketAddress.sin_addr.s_addr, server->h_length);    

    // Checking connection
    int status = connect(client, (struct sockaddr *) &socketAddress, sizeof(socketAddress));
    if (status < 0)
    {
        cout << "Error: Connection error -> " << strerror(errno) << endl;
        exit(4);
    }
    
    // Checking ability to send message to server
    char buffer[max];
    strcpy(buffer, argv[3]);
    int count = send(client, buffer, strlen(buffer), 0);    // send count
    if (count < 0)
    {
        cout << "Error: Cannot send message to server." << endl;
        exit(5);
    }

    bzero(buffer, max);     // erasing buffer data

     // Checking ability to receive message from server
    int receive = recv(client, buffer, max, 0);
    if (receive < 0)
    {
        cout << "Error: Cannot Receive message from server." << endl;
        exit(6);
    }

    // Checking if the specified file name can be opened by client
    if (strcmp(buffer, "OPEN") != 0)
    {
        cout << "Error: Client could not successfully open the specified input file." << endl;
        exit(7);
    }

    bzero(buffer, max);     // erasing buffer data

    // Checking if the SEND message can be sent to server
    strcpy(buffer, "SEND");      // proceed after client receives message
    int countSend = send(client, buffer, strlen(buffer), 0);
    if (countSend < 0)
    {
        cout << "Error: Unable to send message to server." << endl;
        exit(8);
    }

    bzero(buffer, max);     // erasing buffer data

    while(static_cast<int> (recv(client, buffer, max, 0)) > 0)
    {
        buffer[64] = '\0';      // null terminating char
        write(1, buffer, strlen(buffer));       // write contents
        bzero(buffer, max);     // erasing buffer data for next loop
    }

    if (static_cast<int> (recv(client, buffer, max, 0)) < 0)
    {
        cout << "Error: Unable to receive file content message from server." << endl;
        exit(9);
    }

    close(client);      // close program
}