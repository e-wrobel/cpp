//
//  agent_unix.cpp
//  
//
//  Created by Marcin Wróblewski on 30.10.2016.
//
//
// Size for internal buffers:
#define SIZE 20480

// Size for socket buffers, should be <= SIZE:
#define BSIZE 20480

#include <string.h>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fstream>

using namespace std;

class Agent
{
public:
    int s, c, pid;
    struct sockaddr_in server, client;
    socklen_t clilen;
    char *getbuf;
    const char *sendbuf;
    char *commandbuf;
    char *logname;
    int l1, l2;
    string confile;
    char *authstring;
    int std_out;
    FILE *fdlog;
    int port;
// Methods section
    Agent();
    ~Agent();
    int Server();
    void Time();
    int GetConfig();
    int VerifyAuth();
    int Communication();
    int ExecuteCommand(char *c);
    
    static void sigchld_handler(int unused)
    {
        while (waitpid(-1, NULL, WNOHANG) > 0);
    }
};

// Creating buffers, server port, and logfile name:
Agent::Agent()
{
    getbuf = new char[SIZE];
    sendbuf = new char[SIZE];
    commandbuf = new char[SIZE];
    confile = "agent.conf";
    sendbuf = "COMMAND";
//    port = 8000;
}

Agent::~Agent()
{
    delete [] getbuf;
    delete [] sendbuf;
    delete [] commandbuf;
    fflush(NULL);
    fclose(fdlog);
//    dup2(std_out,1);
}

// Get config items:
int Agent::GetConfig()
{
    
    string line;
    string delimiter = "=";
    const char  *log;
    const char *pass;
    const char *port_tmp;
    
    ifstream myfile( confile );
    cout << "Configuration file: " << confile << endl;
    cout << "Reading configuration: "<< endl << endl;

    if (myfile)
    {
        while (getline( myfile, line ))
        {
            if ( strstr(line.c_str(), "passwd") )
            {
                pass = line.substr(line.find(delimiter) + 2  ).c_str();
                authstring = strdup(pass);
                cout << "Password: " << authstring << endl;
            }
            else if
                ( strstr(line.c_str(), "log") )
            {
                log = line.substr(line.find(delimiter) + 2  ).c_str();
                logname = strdup(log);
                cout << "Log file: " << logname << endl ;
            }
            else if
                ( strstr(line.c_str(), "port") )
            {
                port = stoi(line.substr(line.find(delimiter) + 2 ));
                cout << "Port: " << port << endl ;
            }

        }
        cout << "Configuration was read." << endl << endl;
        myfile.close();
    }
    else cout << "Please check agent.conf\n";
    
    return 0;

}

// Get authentication items:
int Agent::VerifyAuth()
{
//    authstring = "password1";
    l1 = strlen(authstring);
    l2 = strlen(getbuf);
//    if (l1 != l2)
    if ( string(authstring) == string(getbuf))
        {
            return 0;
        }
    else
        {
            return 1;
        }
}

// Time method:
void Agent::Time()
{
    
    struct tm *current;
    time_t now;
    time(&now);
    current = localtime(&now);
    printf("\n%i:%i:%i\n", current->tm_hour, current->tm_min, current->tm_sec);
    
}
// Executing obtained script:
int Agent::ExecuteCommand(char *c)
{
    printf("Executing command: {\n\n%s\n\n}\n\n", c);
    char tmp[100];
    FILE *pipe = popen(c, "r");
    string output = "";
    while (fgets(tmp, sizeof(tmp), pipe) != NULL)
    {
        output = output + tmp;
    }
    pclose(pipe);

    sendbuf = output.c_str();
    return 0;
}

// Communication protocol:
int Agent::Communication()
{
    int server_read, server_write;
    fdlog = fopen(logname, "a");
    if (! fdlog)
        {
            cout << "Error openning log file!\n";
        }

    std_out = dup(1);
    close(1);
    dup2(fileno(fdlog),1);
    Time();
// Communication:
    
// Client sends auth:
    server_read = read(c, getbuf, BSIZE);
    if (server_read < 0)
    {
        puts("Error reading from socket!");
    }
    else
    {
        printf("Auth received from client: %s\n", getbuf);
    }

    // Server is validating auth:
    if (VerifyAuth() != 0)
    {
        printf("Auth error! Closing communication.");
    }
    else
        {
            server_write = write(c, sendbuf, BSIZE);
            if (server_write < 0)
                {
                    puts("Error sending!");
                    }
            else
                {
                    printf("Please provide commands.\n");
                }
            
            server_read = read(c, commandbuf, BSIZE);
            if (server_read < 0)
            {
                puts("Error reading from client!");
            }
            else
            {
                printf("Commands provided: {\n\n%s\n\n}\n\n", commandbuf);
                ExecuteCommand(commandbuf);
// Sending output to the Client:
                server_write = write(c, sendbuf, BSIZE);
                if (server_write < 0)
                {
                    puts("Error sending to client!");
                }
                else
                {
                    printf("Output sent to client: {\n\n%s\n\n}\n\n", sendbuf);
                }
            }
        }
    printf("\n");
    dup2(std_out,1);
    
    cout << "Closing communication."<< endl;
    return 0;
}

// Multiprocess TCP server stack:
int Agent::Server()
{
    
    signal(SIGCHLD, sigchld_handler);
    cout << "Initializing agent." << endl;
    
    // Creating socket:
    if((s = socket(AF_INET , SOCK_STREAM , 0 )) < 0)
    {
        printf("Could not create server socket!");
    }
    else
    {
        printf("Socket created.\n");
    }
    
    c = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &c, sizeof(c));
// Setuping network struct:
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_family = AF_INET;
    server.sin_port = htons( port );
    
// Binding socket to port:
    if (bind(s, (struct sockaddr *) &server,sizeof(server)) < 0)
    {
        puts("Error on binding!");
        return 1;
    }
    else
    {
        printf ("Binding to port %i.\n", port);
    }
    // Listen for connections:
    cout << "Server started." << endl << endl;
    listen(s, 5);
    
    while (1)
    {
        clilen = sizeof(client);
        if ((c = accept(s, (struct sockaddr *) &client, &clilen)) < 0)
        {
            printf("Could not create client socket!");
            return 1;
        }
        pid = fork();
        if (pid < 0)
        {
            puts("Error on fork!");
        }
        if (pid == 0)
        {
            close(s);
            
            // Whole client-server communication:
            
            Communication();
            _exit(0);
            // End of whole client-server communication.
            
        }
        else close(c);
    }
    getchar();
    return 0;

}

int main()
{
    Agent agent;
    agent.GetConfig();
    agent.Server();
    return 0;
}
