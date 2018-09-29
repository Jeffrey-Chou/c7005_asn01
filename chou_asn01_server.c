#include "chou_asn01_shared.h"


int main(int argc, char* argv[])
{
    int n;
    unsigned int client_len;
    int listen_sd, new_sd, transfer_sd;
    long int filesize;
    struct sockaddr_in server, client;
    char recvBuff[BUFFSIZE], sendBuff[BUFFSIZE], filename[63];
    FILE* fp;

    if((listen_sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Cannot create socket");
        exit(1);
    }

    

    memset(&server, 0, sizeof (struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_LISTEN_PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(listen_sd, (struct sockaddr*)&server, sizeof(server)) == -1)
    {
        perror("Can't bind name to socket");
        exit(1);
    }
    listen(listen_sd, 5);


    while (TRUE)
    {
        client_len = sizeof(client);

        if( (new_sd = accept (listen_sd, (struct sockaddr*)&client, &client_len) )  == -1)
        {
            fprintf(stderr, "Can't accept client\n");
            exit(1);
        }

        

        printf("Remote Address: %s\n", inet_ntoa(client.sin_addr));

        client.sin_port = htons(CLIENT_LISTEN_PORT);
        while(TRUE)
        {
            n = recv(new_sd, recvBuff, REQ_PACK, MSG_WAITALL);

            if(n <= 0)
            {
                close(new_sd);
                break;
            }

            if((transfer_sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
            {
                perror("Cannot create socket");
                exit(1);
            }
            printf("%s\n",recvBuff);
            if(recvBuff[0] == '1')
            {

                sscanf(&recvBuff[1], "%s", filename);
                
                fp = fopen(filename, "r");
                if( connect(transfer_sd, (struct sockaddr *)&client, sizeof(client)) == -1)
                {
                    fprintf(stderr, "Can't connect to client\n");
                    perror("connect");
                    continue;
                }
                if(fp == NULL)
                {
                    sprintf(sendBuff, "%d", -1);
                    send(transfer_sd, sendBuff, REQ_PACK, 0);
                    continue;

                }

                filesize = getFileSize(fp);
                sprintf(sendBuff, "%ld", filesize);
                send(transfer_sd, sendBuff, REQ_PACK, 0);
                sendFile(transfer_sd, sendBuff, fp);
                fclose(fp);
                close(transfer_sd);

            }

            if(recvBuff[0] == '2')
            {
                if( connect(transfer_sd, (struct sockaddr *)&client, sizeof(client)) == -1)
                {
                    fprintf(stderr, "Can't connect to client\n");
                    perror("connect");
                    continue;
                }
                sscanf(&recvBuff[1], "%s", filename);
                recv(transfer_sd, recvBuff, REQ_PACK, 0);
                sscanf(recvBuff, "%ld", &filesize);
                fp = fopen(filename, "w");
                receiveFile(transfer_sd, recvBuff, fp, filesize);
                fclose(fp);
            }
        }
        
    }

    close (listen_sd);
    return 0;
}