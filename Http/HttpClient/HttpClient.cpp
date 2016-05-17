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


// #include <stdio.h>      /* for printf(), fprintf() */
// // #include <winsock.h>    /* for socket(),... */
// #include <stdlib.h>     /* for exit() */
// #include <winsock2.h>    /* for socket(),... */
// #include <ws2tcpip.h>    /* for socket(),... */
//
// #include <iostream>
// using namespace std;
//
// #pragma comment(lib, "Ws2_32.lib")

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#include <iostream>
using namespace std;


#define DEFAULT_PORT "80"

// void DieWithError(char *errorMessage);  /* Error handling function */



int main(int argc, char *argv[])
{
    char* HTTPServPort;     /* Echo server port */
    WSADATA wsaData;                 /* Structure for WinSock setup communication */
    WORD wVersion = MAKEWORD(2, 2); // Used to request version 2.2 of Windows sockets
    int iResult;                    // Error check if WSAStartup
    struct addrinfo *result = NULL,
                       *ptr = NULL,
                       hints;
    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if ((argc < 2) || (argc > 3))    /* Test for correct number of arguments */
    {
        // fprintf(stderr, "Usage: %s <Server IP> [<Echo Word>] [<Echo Port>]\n", argv[0]);
        fprintf(stderr, "Usage: %s <Server IP> [<Echo Port>]\n", argv[0]);
        exit(1);
    }

    if (argc == 4)
        HTTPServPort = argv[2]; /* Use given port, if any */
    else
        HTTPServPort = DEFAULT_PORT;

    if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) /* Load Winsock 2.0 DLL */
    {
        fprintf(stderr, "WSAStartup() failed");
        exit(1);
    }

    // Resolve the server address and port
    iResult = getaddrinfo(argv[1], HTTPServPort, &hints, &result);
    if (iResult != 0) {
        cout << "getaddrinfo failed: " << iResult << endl;
        WSACleanup();
        return 1;
    }

     SOCKET ClientSocket = INVALID_SOCKET;

     // Attempt to connect to the first address returned by
    // the call to getaddrinfo
    ptr = result;

    // Create a SOCKET for connecting to server
    ClientSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

    if (ClientSocket == INVALID_SOCKET) {
        cout << "Error at socket(): " << WSAGetLastError() << endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Connect to server.
    iResult = connect( ClientSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        closesocket(ClientSocket);
        ClientSocket = INVALID_SOCKET;
    }

    // Should really try the next address returned by getaddrinfo
    // if the connect call failed
    // But for this simple example we just free the resources
    // returned by getaddrinfo and print an error message

    freeaddrinfo(result);

    if (ClientSocket == INVALID_SOCKET) {
        cout << "Unable to connect to server!\n";
        WSACleanup();
        return 1;
    }

    #define DEFAULT_BUFLEN 512

    int recvbuflen = DEFAULT_BUFLEN;

    char *sendbuf = " GET / HTTP/1.0\n\n";
    char recvbuf[DEFAULT_BUFLEN] = "";

    // Send an initial buffer
    iResult = send(ClientSocket, sendbuf, (int) strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
      cout << "send failed: " << WSAGetLastError() << endl;
      closesocket(ClientSocket);
      WSACleanup();
      return 1;
    }

    cout << "Bytes Sent: " << iResult << endl;


    // shutdown the connection for sending since no more data will be sent
    // the client can still use the ClientSocket for receiving data
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        cout << "shutdown failed: " << WSAGetLastError() << endl;
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // Receive data until the server closes the connection
    do {
       iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
       if (iResult > 0){
          cout << "Bytes received: " << iResult << endl;
          cout << recvbuf << endl;
       }
       else if (iResult == 0)
          cout << "Connection closed\n";
       else
          cout << "recv failed: " << WSAGetLastError() << endl;
    } while (iResult > 0);

    // cleanup
    closesocket(ClientSocket);

    return 0;
}
