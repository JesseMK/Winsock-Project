/* CHANGES FROM UNIX VERSION                                                   */
/*                                                                             */
/* 1.  Changed header files.                                                   */
/* 2.  Added WSAStartUP() and WSACleanUp().                                    */
/* 3.  Used closesocket() instead of close().                                  */

// TCPechoClient.cpp : Defines the entry point for the console application.
//

//=================================================== file = TCPechoClient.c =====
//=  A simple echo client to demonstrate sockets programming                  =
//=   - TCP/IP client/server model is implemented                             =
//=============================================================================
//=  Notes:
//=  TCPechoServer should be running.                                         =
//=---------------------------------------------------------------------------=
//=  Example execusion:
//=
//=  TCPechoClient 192.168.100.101 Hello 2016                                 =
//=============================================================================


#include <stdio.h>      /* for printf(), fprintf() */
#include <winsock.h>    /* for socket(),... */
#include <stdlib.h>     /* for exit() */

#pragma comment(lib, "Ws2_32.lib")

#define RCVBUFSIZE 32   /* Size of receive buffer */

void DieWithError(char *errorMessage);  /* Error handling function */

void main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    unsigned short echoServPort;     /* Echo server port */
    char *servIP;                    /* Server IP address (dotted quad) */
    char *echoString;                /* String to send to echo server */
    char echoBuffer[RCVBUFSIZE];     /* Buffer for echo string */
    int echoStringLen;               /* Length of string to echo */
    int bytesRcvd, totalBytesRcvd;   /* Bytes read in single recv() and total bytes read */
    WSADATA wsaData;                 /* Structure for WinSock setup communication */
    char out_buff[10];
    char in_buff[10];

    // if ((argc < 3) || (argc > 4))    /* Test for correct number of arguments */
    if ((argc < 2) || (argc > 3))    /* Test for correct number of arguments */
    {
        // fprintf(stderr, "Usage: %s <Server IP> [<Echo Word>] [<Echo Port>]\n", argv[0]);
        fprintf(stderr, "Usage: %s <Server IP> [<Echo Port>]\n", argv[0]);
        exit(1);
    }

    servIP = argv[1];             /* First arg: server IP address (dotted quad) */

    if (argc == 4)
        echoServPort = atoi(argv[2]); /* Use given port, if any */
        // echoServPort = atoi(argv[3]); /* Use given port, if any */
    else
        echoServPort = 7;  /* otherwise, 7 is the well-known port for the echo service */

    if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) /* Load Winsock 2.0 DLL */
    {
        fprintf(stderr, "WSAStartup() failed");
        exit(1);
    }

    /* Create a reliable, stream socket using TCP */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));     /* Zero out structure */
    echoServAddr.sin_family      = AF_INET;             /* Internet address family */
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
    echoServAddr.sin_port        = htons(echoServPort); /* Server port */
    /* Establish the connection to the echo server */
    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("connect() failed");
    else
        printf("Connect Successful\n");

    while (1)
    {
        printf("Enter your message > ");
        gets(in_buff);

        if (!strcmp(in_buff, "exit"))
        {
            closesocket(sock);
            WSACleanup();  /* Cleanup Winsock */
            exit(0);
        }

        // printf("get %s\n", in_buff);

        echoStringLen = strlen(in_buff);          /* Determine input length */

        /* Send the string, including the null terminator, to the server */
        if (send(sock, in_buff, echoStringLen, 0) != echoStringLen)
            DieWithError("send() sent a different number of bytes than expected");

        /* Receive the same string back from the server */
        totalBytesRcvd = 0;
        printf("Received: ");                /* Setup to print the echoed string */
        while (totalBytesRcvd < echoStringLen)
        {
            /* Receive up to the buffer size (minus 1 to leave space for
               a null terminator) bytes from the sender */
            if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0)
                DieWithError("recv() failed or connection closed prematurely");
            totalBytesRcvd += bytesRcvd;   /* Keep tally of total bytes */
            echoBuffer[bytesRcvd] = '\0';  /* Add \0 so printf knows where to stop */
            printf("%s", echoBuffer);            /* Print the echo buffer */
        }

        printf("\n");    /* Print a final linefeed */
    }

    closesocket(sock);
    WSACleanup();  /* Cleanup Winsock */

    exit(0);
}
