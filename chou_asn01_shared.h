#pragma once

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define TRUE 1
#define SERVER_LISTEN_PORT 7005
#define CLIENT_LISTEN_PORT 7006
#define REQ_PACK 64
#define BUFFSIZE 256

long int getFileSize(FILE* fp);
void receiveFile(int sd, char *buffer, FILE *fp, long int filesize);
void sendFile(int sd, char *buffer, FILE *fp);
void initSockAddr(struct sockaddr_in *addr, int);