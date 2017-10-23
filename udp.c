#include<stdio.h>
#include<winsock2.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <inttypes.h>

#define BUFLEN 512  //Max length of buffer
#define PORT 1000   //The port on which to listen for incoming data
#define DATA 8


char reply[2] = {183, 0};
char buf[BUFLEN];
int flag = 0;
SOCKET s;
struct sockaddr_in server, si_other;
int slen , recv_len;

WSADATA wsa;
slen = sizeof(si_other);

void configSocket( )
{
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0){
        printf("Failed. Error Code : %d",WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    //Create a socket
    if((s = socket(AF_INET , SOCK_DGRAM , 0 )) == INVALID_SOCKET){
        printf("Could not create socket : %d" , WSAGetLastError());
    }

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( PORT );
    //Bind
    if( bind(s ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR){
        printf("Bind failed with error code : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }
}


void receptionUDP( uint8_t frame[] ){
    int i;
    memset(frame,'\0', BUFLEN);
    if ((recv_len = recvfrom(s, frame, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR){
        printf("recvfrom() failed with error code : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("Packet recieved: rx ");
    for(i=0;i<recv_len;i++)
    {
        printf("%02x-",frame[i]);
    }
    printf("\n\r");
}

void responUDP( char frame[])
{
    if (sendto(s, frame, 7, 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR){
                    printf("sendto() failed with error code : %d" , WSAGetLastError());
                    exit(EXIT_FAILURE);
                }

}





