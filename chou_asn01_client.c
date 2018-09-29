#include "chou_asn01_shared.h"

#include <netdb.h>
#define OPTIONS "?h:"


int main(int argc, char* argv[])
{
    int sd, listen_sd, transfer_sd;
    int opt;
    long int filesize;
    struct hostent *hp;
    struct sockaddr_in server, transfer;
    char *host = NULL, **pptr;
    char str[16], buf[REQ_PACK - 1], sendBuff[BUFFSIZE], recvBuff[BUFFSIZE];
    FILE *fp;

    while( (opt = getopt(argc, argv, OPTIONS)) != -1)
    {
        switch (opt)
        {
            case 'h':
            host = optarg;
            break;

        }
    }

    if(host == NULL)
    {
        return 1;
    }

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Cannot create socket");
        exit(1);
    }

    if((listen_sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Cannot create listen socket");
        exit(1);
    }

    memset(&transfer, 0, sizeof (struct sockaddr_in));
    transfer.sin_family = AF_INET;
    transfer.sin_port = htons(CLIENT_LISTEN_PORT);
    transfer.sin_addr.s_addr = htonl(INADDR_ANY);

    

    if(bind(listen_sd, (struct sockaddr*)&transfer, sizeof(transfer)) == -1)
    {
        perror("Can't bind name to socket");
        exit(1);
    }

    listen(listen_sd, 5);

    memset(&server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_LISTEN_PORT);
    if((hp = gethostbyname(host)) == NULL)
    {
        fprintf(stderr, "Unknown server address\n");
        exit(1);
    }

    

    memcpy((char *)&server.sin_addr, hp->h_addr, hp->h_length);

    if ( connect(sd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        fprintf(stderr, "Can't connect to server\n");
        perror("connect");
        exit(1);
    }

    printf("Connected:    Server Name: %s\n", hp->h_name);
	pptr = hp->h_addr_list;
    printf("\t\tIP Address: %s\n", inet_ntop(hp->h_addrtype, *pptr, str, sizeof(str)));

    while(TRUE)
    {

        printf("Enter 1 to get a file, 2 to send a file, 0 to exit\n");
        int c = getchar();
        getchar();

        if(c < 48 || c > 50)
        {
            continue;
        }

        if(c == '0')
        {
            printf("Exiting program\n");
            break;
        }

        printf("Enter a filename\n");
        fgets(buf, REQ_PACK - 1, stdin);

        buf[strcspn(buf, "\n")] = 0;
        
        
        if(c == '1')
        {
            sprintf(sendBuff, "%c%s", c, buf);
            send(sd, sendBuff, REQ_PACK, 0);
            if(( transfer_sd = accept(listen_sd, NULL, NULL)) == -1)
            {
                fprintf(stderr, "Can't accept data transfer\n");
                continue;
            }
            recv(transfer_sd, recvBuff, REQ_PACK, 0);
            sscanf(recvBuff, "%ld", &filesize);
            if(filesize < 0)
            {
                printf("File not found on server\n");
                close(transfer_sd);
                continue;
            }
            fp = fopen(buf, "w");
            receiveFile(transfer_sd, recvBuff, fp, filesize);
            fclose(fp);
            close(transfer_sd);
            
        }
        if(c == '2')
        {
            fp = fopen(buf, "r");
            if(fp == NULL)
            {
                printf("file does not exist\n");
                continue;
            }
            sprintf(sendBuff, "%c%s", c, buf);
            send(sd, sendBuff, REQ_PACK, 0);
            
            if(( transfer_sd = accept(listen_sd, NULL, NULL)) == -1)
            {
                fprintf(stderr, "Can't accept data transfer\n");
                continue;
            }
            filesize = getFileSize(fp);
            sprintf(sendBuff, "%ld", filesize);
            send(transfer_sd, sendBuff, REQ_PACK, 0);
            sendFile(transfer_sd, sendBuff, fp);
            fclose(fp);
            close(transfer_sd);
        }


    }

    close(sd);
    return 0;
}