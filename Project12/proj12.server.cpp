/*******************************************************************************
		Winnie Yang
    Computer Project #12
    Server file
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
#include <sys/stat.h>
#include <fcntl.h>

#define BSIZE 64      // buffer is 64 bytes max

using namespace std;

/*---------------------------------------------------------------------------
    Name:  main
    Purpose: Do all the work for this project
    Receive: Number of arguments int, arguments in char*
    Return: None
---------------------------------------------------------------------------*/
int main(int argc, char* argv[]) 
{
    if (argc > 1)
    {
        cout << "No need to provide arguments for server file." << endl;
    }

    int listen_sd = socket( AF_INET, SOCK_STREAM, 0 );
    if (listen_sd < 0) 
    {
        perror( "socket" );
        exit( 1 );
    }

    struct sockaddr_in saddr;

    bzero( &saddr, sizeof(saddr) );
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons( 0 );
    saddr.sin_addr.s_addr = htonl( INADDR_ANY );

    char hostname[64];
    gethostname(hostname, 64);
    char buffer[BSIZE];         // buffer variable

    int bstat = bind( listen_sd, (struct sockaddr *) &saddr, sizeof(saddr) );
    if (bstat < 0)
    {
        perror( "bind" );
        exit( 2 );
    }

    socklen_t saddrSize = sizeof(saddr);
    getsockname(listen_sd, (struct sockaddr *) &saddr, &saddrSize);

    int lstat = listen( listen_sd, 5 );
    if (lstat < 0)
    {
        perror( "listen" );
        exit( 3 );
    }
    printf( "Server listening on fd %d\n\n", listen_sd );

    struct hostent* host = gethostbyname(hostname);
    cout << host->h_name << "  " << ntohs(saddr.sin_port) << endl;

    struct sockaddr_in caddr;
    unsigned int clen = sizeof(caddr);

    int comm_sd = accept( listen_sd, (struct sockaddr *) &caddr, &clen );
    if (comm_sd < 0)
    {
        perror( "accept" );
        exit( 4 );
    }
    printf( "Server connected on fd %d\n\n", comm_sd );

    bzero( buffer, BSIZE );
    int nrecv = recv( comm_sd, buffer, BSIZE, 0 );
    if (nrecv < 0) 
    {
        perror( "recv" );
        exit( 5 );
    }
    printf( "Server received %d bytes\n", nrecv );
    printf( "Message: %s\n", buffer );

    const char *openfile = "OPEN";
    const char *failed = "FAILED";
    int nsend;

    int opfd = open(buffer, O_RDONLY, S_IRUSR);
    if (opfd < 0)
    {
        nsend = send( comm_sd, failed, strlen(failed), 0 );
        if (nsend < 0)
        {
            perror("send");
            exit( 6 );
        }
        exit( 7 );
    }

    nsend = send( comm_sd, openfile, strlen(openfile), 0 );
    if (nsend < 0) 
    {
        perror( "send" );
        exit( 8 );
    }
    printf( "Server sent %d bytes\n", nsend );
    printf( "Message: %s\n", buffer );

    bzero(buffer, BSIZE);
    nrecv = recv(comm_sd, buffer, BSIZE, 0);
    if (nrecv < 0)
    {
        perror( "receive" );
        exit( 9 );
    }

    if (strcmp(buffer, "SEND") != 0)
    {
        cout << "Error: Did not receive the SEND message." << endl;
        exit( 10 );
    }

    while (1)
    {
        bzero(buffer, BSIZE);
        ssize_t nbytes = read(opfd, buffer, BSIZE);
        if (nbytes < 0)
        {
            perror( "read" );
            exit( 11 );
        }
        if (nbytes == 0)
        {
            break;
        }

        nsend = send(comm_sd, buffer, nbytes, 0);
        if (nsend < 0)
        {
            perror( "send" );
            exit( 12 );
        }
    }

    printf( "Server closed fd %d\n\n", comm_sd );

    close( comm_sd );
    close( listen_sd );
}
