#define SIZE 20480
#define BSIZE 20480

#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

int main(int argc, char* argv[])
{
    int s;
    struct sockaddr_in server; // we are using this tructure to connect to server
    char getbuf[SIZE];
    
    if (argc != 3)
    {
        cout << "Usage: ./client 'command1; command2' passtring" << endl;
        exit(1);
    }
    
    char *putbuf = argv[2];
    char *command = argv[1];

    const char *ip = "192.168.0.101";
    int port = 8000;
    int recv_size;
    
    if((s = socket(AF_INET , SOCK_STREAM , 0 )) < 0)
    {
        printf("Could not create socket!");
    }
    else
    {
    printf("Socket created.\n");
    }
    
    // Remote server parameters:
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_family = AF_INET;
    server.sin_port = htons( port );
    
    // Connecting to the server
    if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        puts("Connection error, please check if agent is started!");
        return 1;
    }
    else
    {
	printf ("Connected to %s\n", ip);
    }

    // Sending  auth to the server
    if(send(s, putbuf, strlen(putbuf), 0) < 0)
    {
        puts("Send failed\n");
        return 1;
    }
    else
    {
    	printf("Message sent: %s\n", putbuf);
    }
	
    //Received a message from the server
    if(recv(s, getbuf, SIZE, 0) < 0)
    {
        puts("recv failed");
    }
    else
    {
    	printf("Received communication from server.\n");
        if (strlen(getbuf) == 0)
        {
            cout << "Auth error!" << endl;
            exit(1);
        }
    }
    
    // Sending  command to the server
    if(send(s, command, strlen(command), 0) < 0)
    {
        puts("Send failed\n");
        return 1;
    }
    else
    {
        printf("Commands sent: %s\n", command);
    }
    
    //Received a message from the server
    if(recv(s, getbuf, SIZE, 0) < 0)
    {
        puts("recv failed");
    }
    else
    {
        printf("Received output: %s\n", getbuf);
    }
    
    if ( strlen(getbuf) == 0)
    {
    cout << endl << "Command error!"<< endl;
    }
    
    return 0;
}
