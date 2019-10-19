//
//  agent.hpp
//  Authenticator
//
//  Created by Marcin Wróblewski on 19/10/2019.
//  Copyright © 2019 Marcin Wróblewski. All rights reserved.
//
#pragma once

#ifndef agentBase_hpp
#define agentBase_hpp

#include <iostream>
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
#include <sys/wait.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>

class AgentBase {
public:
    int s, c, pid;
    struct sockaddr_in server, client;
    socklen_t clilen;
    SSL_CTX *sslctx;
    SSL *cSSL;
    char *getbuf;
    char *sendbuf;
    char *commandbuf;
    char *logname;
    string confile;
    char *authstring;
    int std_out;
    FILE *fdlog;
    int port;

    AgentBase();
    ~AgentBase();

    int Server();
    void Time();
    int GetConfig();
    int VerifyAuth();
    int Communication();
    int ExecuteCommand(char *c);
    void InitializeSSL();
  
    static void catch_child(int sig_num);
    int Initialize();
};

#endif /* agent_hpp */


