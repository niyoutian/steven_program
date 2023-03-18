#include <stdio.h>
#include <openssl/ssl.h>

#define SERVER_IP  "172.100.10.165"
#define SERVER_PORT 3496
#define CLIENT_IP  "172.100.10.167"

#define CA_CERT_FILE     "/home/steven/cert_mgr/certs/ca_cert.pem"
#define SERVER_CERT_FILE "/home/steven/cert_mgr/certs/server_cert.pem"
#define CLIENT_CERT_FILE "/home/nancy/cert_mgr/certs/client_cert.pem"
#define SERVER_KEY_FILE  "/home/steven/cert_mgr/private/server_key.pem"
#define CLIENT_KEY_FILE  "/home/nancy/cert_mgr/private/client_key.pem"

int initSslLibrary(unsigned int role);
int bindSslAndConnect(int sockfd);
int bindSslAndAccept(int sockfd);
SSL* getSslFd(void) ;