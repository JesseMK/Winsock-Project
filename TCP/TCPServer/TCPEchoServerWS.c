/*                                                                             */
/*===========File name is TCPechoServer========================================*/
/* This program creates a simple server with accept(),send back the message    */
/* immediately after it receives from the client.                              */
/* Execusion:                                                                  */
/*     TCPechoServer portnumber                                                */
/*e.g. TCPechoServer  2016                                                     */

#include <stdio.h>      /* for printf(), fprintf() */
#include <winsock.h>    /* for socket(),... */
#include <stdlib.h>     /* for exit() */

#pragma comment(lib, "Ws2_32.lib")

#define MAXPENDING 5    /* Maximum outstanding connection requests */

void DieWithError(char *errorMessage);  /* Error handling function */
void HandleTCPClient(int clntSocket);   /* TCP client handling function */

void main(int argc, char *argv[])
{
    int servSock;                    /* Socket descriptor for server */
    int clntSock;                    /* Socket descriptor for client */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned short echoServPort;     /* Server port */
    unsigned int clntLen;            /* Length of client address data structure */
    WSADATA wsaData;                 /* Structure for WinSock setup communication */

    if (argc != 2)     /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]);  /* first arg:  Local port */

    if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) /* Load Winsock 2.0 DLL */
    {
        fprintf(stderr, "WSAStartup() failed");
        exit(1);
    }

    /* Create socket for incoming connections */
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");


    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");

    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, MAXPENDING) < 0)
        DieWithError("listen() failed");

        // int 		       rc;              // Return code for setsockopt()
        // int 		       newTCPWin;       // TCP window size - as requested
        // int 		       outTCPWin;       // TCP window size - as set
        // int	               len;   	     	// The size, in bytes, of the buffer pointed to by the socket 'option'value parameter
        //
        //
        // // adjust TCP window size (SO_RCVBUF size) for this socket only
        // newTCPWin = 8192;
        // len=sizeof(int);
        //
        // // set SO_RCVBUF
        // rc = setsockopt(servSock, SOL_SOCKET, SO_RCVBUF,(char *)&newTCPWin, len);
        // if ( rc < 0 )
        //     { printf("setsockopt() failed...\n"); exit(1);}
        //
        // // check if SO_RCVBUF was set correctly
        // rc = getsockopt(servSock, SOL_SOCKET, SO_RCVBUF, (char *)&outTCPWin, &len);
        // if ( rc < 0 )
        //     printf("getsockopt() failed: with return code = %d\n", WSAGetLastError());
        // printf("Initial TCP Window size is: %d.\n", outTCPWin);
        // if ( outTCPWin != newTCPWin )
        //     printf( "WARNING: Attempted to set window size to: %d, but got %d.", outTCPWin, newTCPWin);



    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        clntLen = sizeof(echoClntAddr);

        /* Wait for a client to connect */
        if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0)
            DieWithError("accept() failed");

        /* clntSock is connected to a client! */

        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

        HandleTCPClient(clntSock);
    }
    /* NOT REACHED */
}
