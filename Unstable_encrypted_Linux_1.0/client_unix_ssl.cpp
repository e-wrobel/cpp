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
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>
using namespace std;

void InitializeSSL()
{
    ERR_load_BIO_strings();
    ERR_load_crypto_strings();
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();
}

int main(int argc, char* argv[])
{
    int s;
    struct sockaddr_in server; // we are using this tructure to connect to server
    SSL_CTX *sslctx;
    SSL *cSSL;
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
    InitializeSSL();
    sslctx = SSL_CTX_new( SSLv23_server_method());
    if (! sslctx)
    {
	    cout << "Error creating CTX!" << endl;
            exit(0);
    }

   int use_cert = SSL_CTX_use_certificate_file(sslctx, "2_poczta.e-wrobel.pl.crt" , SSL_FILETYPE_PEM);
   if (use_cert <= 0)
   {
	   cout << "Error setting cert file!" << endl;
   }

   int use_prv = SSL_CTX_use_PrivateKey_file(sslctx, "priv.key", SSL_FILETYPE_PEM);

   if (use_prv <= 0)
   {
	   cout << "Error setting key file!" << endl;
   }
 
/*Make sure the key and certificate file match*/
   if (SSL_CTX_check_private_key(sslctx) == 0) 
   {
      	   printf("Private key does not match the certificate public key\n");
           exit(0);
   }

 SSL_CTX_set_verify( sslctx, SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT|SSL_VERIFY_NONE, NULL );
 cSSL = SSL_new(sslctx);

 if (!cSSL)
 {
	 cout << "Error creating SSL structure!" << endl;
 }
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

SSL_set_fd(cSSL, s );
int err = SSL_connect(cSSL);
if (err<1)
{
   printf("SSL error #%d in accept,program terminated\n",SSL_get_error(cSSL, err));
     if(err==5){printf("sockerrno is:%d\n",err);}
       close(s);
       SSL_free(cSSL);
       SSL_CTX_free(sslctx);
       exit(0);
}

cout << "sending auth to server" << endl;
    // Sending  auth to the server
    SSL_set_connect_state(cSSL);
    if(SSL_write(cSSL, putbuf, SIZE) < 0)
    {
        puts("Send failed\n");
        return 1;
    }
    else
    {
    	printf("Message sent: %s\n", putbuf);
    }
	
    //Received a message from the server
    if(SSL_read(cSSL, getbuf, SIZE) < 0)
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
    if(SSL_write(cSSL, command, strlen(command)) < 0)
    {
        puts("Send failed\n");
        return 1;
    }
    else
    {
        printf("Commands sent: %s\n", command);
    }
    
    //Received a message from the server
    if(SSL_read(cSSL, getbuf, SIZE) < 0)
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
    SSL_free(cSSL);
    return 0;
}
