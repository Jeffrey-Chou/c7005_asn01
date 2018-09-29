#include "chou_asn01_shared.h"

long int getFileSize(FILE *fp)
{
    long int size = 0;
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    return size;
}

void receiveFile(int sd, char *buffer, FILE *fp, long int filesize)
{
    int n;
    long int total = 0;
    while(total < filesize)
    {
        n = recv(sd, buffer, BUFFSIZE,0);
        total += n;
        fwrite(buffer, 1, n, fp);
    }
}

void sendFile(int sd, char *buffer, FILE *fp)
{
    int n = 0;
    while(!feof(fp))
    {
        n = fread(buffer, sizeof(char), BUFFSIZE, fp);
        send(sd, buffer, n, 0);
    }
}
