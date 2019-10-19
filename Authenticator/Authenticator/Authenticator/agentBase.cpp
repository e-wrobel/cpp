//
//  agent.cpp
//  Authenticator
//
//  Created by Marcin Wróblewski on 19/10/2019.
//  Copyright © 2019 Marcin Wróblewski. All rights reserved.
//

#include "agentBase.hpp"
using namespace std;

// Creating buffers, server port, and logfile name:
AgentBase::AgentBase()
{
    getbuf = new char[SIZE];
    sendbuf = new char[SIZE];
    commandbuf = new char[SIZE];
    confile = "agent.conf";
}

AgentBase::~AgentBase()
{
    delete [] getbuf;
    delete [] sendbuf;
    delete [] commandbuf;
    fflush(NULL);
    fclose(fdlog);
//    dup2(std_out,1);
}

void AgentBase::InitializeSSL()
{
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
};


// Get config items:
int AgentBase::GetConfig()
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
int AgentBase::VerifyAuth()
{
//    authstring = "password1";
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
void AgentBase::Time()
{
    
    struct tm *current;
    time_t now;
    time(&now);
    current = localtime(&now);
    printf("\n%i:%i:%i\n", current->tm_hour, current->tm_min, current->tm_sec);
    
}
// Executing obtained script:
int AgentBase::ExecuteCommand(char *c)
{
   if (strlen(c) != 0)
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
    strcpy(sendbuf, output.c_str());

    return 0;
   }
   else return 1;
}

// Communication protocol:
int AgentBase::Communication()
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
    server_read = SSL_read(cSSL, getbuf, SIZE -1);
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
            server_write = SSL_write(cSSL, "COMMAND", strlen("COMMAND"));
            if (server_write < 0)
                {
                    puts("Error sending!");
                    }
            else
                {
                    printf("Please provide commands.\n");
                }
            
            server_read = SSL_read(cSSL, commandbuf, SIZE -1);
            if (server_read < 0)
            {
                puts("Error reading from client!");
            }
            else
            {
                printf("Commands provided: {\n\n%s\n\n}\n\n", commandbuf);
                ExecuteCommand(commandbuf);
// Sending output to the Client:
                server_write = SSL_write(cSSL, sendbuf, strlen(sendbuf));
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
    
    cout << "Closing communication.\n"<< endl;
    return 0;
}

// Multiprocess TCP server stack:
int AgentBase::Server()
{
    
    cout << "Initializing agent." << endl;
    
    // Creating socket:
    InitializeSSL();
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

    sslctx = SSL_CTX_new( SSLv23_server_method());
    SSL_CTX_set_options(sslctx, SSL_OP_SINGLE_DH_USE);
    int use_cert = SSL_CTX_use_certificate_file(sslctx, "2_poczta.e-wrobel.pl.crt" , SSL_FILETYPE_PEM);
    int use_prv = SSL_CTX_use_PrivateKey_file(sslctx, "priv.key", SSL_FILETYPE_PEM);
    if (use_cert <= 0)
    {
        cout << "Certificate error!" << endl;
    }

    if (use_prv <= 0)
    {
        cout << "Private key error!" << endl;
    }

    if (!SSL_CTX_check_private_key(sslctx))
    {
        cout << "Private key doesn't match certificate!" << endl;
    }


    while (1)
    {
        clilen = sizeof(client);
        if ((c = accept(s, (struct sockaddr *) &client, &clilen)) < 0)
        {
            printf("Could not create client socket!");
            return 1;
        }
    printf ("Connection from IP: %s, port: %i\n", inet_ntoa(client.sin_addr), client.sin_port);
        pid = fork();
        if (pid < 0)
        {
            puts("Error on fork!");
        }
        if (pid == 0)
        {
            close(s);
            
            // Whole client-server communication:
        cSSL = SSL_new(sslctx);
            SSL_set_fd(cSSL, c );
            int ssl_err = SSL_accept(cSSL);
        if(ssl_err != 1)
        {
            //Error occurred, log and close down ssl
            cout << "SSL error!" << endl;
        int err_SSL_get_error = SSL_get_error( cSSL, ssl_err);
        cout <<" err: " << err_SSL_get_error << endl;
            }
            else
        {
            cout << "SSL connection using: " << SSL_get_cipher(cSSL) << endl;
        }
        
        if (SSL_get_peer_certificate(cSSL))
        {
            cout << "This is client certificate: " << endl;
        }
        else
        {
            cout << "Client certificate is NULL!" << endl;
        }

            Communication();
        SSL_shutdown(cSSL);
        SSL_free(cSSL);
            _exit(0);
            // End of whole client-server communication.
            
        }
        else close(c);
    wait(NULL);
    }
    SSL_CTX_free(sslctx);
//    getchar();
    return 0;

}
