#make for ASN01

CC=gcc-8
CFLAGS=-Wall



client: chou_asn01_client.o chou_asn01_shared.o
	$(CC) $(CFLAGS) chou_asn01_client.o chou_asn01_shared.o -o client

server: chou_asn01_server.o chou_asn01_shared.o
	$(CC) $(CFLAGS) chou_asn01_server.o chou_asn01_shared.o -o server

clean:
	rm -f *.o *.bak *.out client server



chou_asn01_shared.o: chou_asn01_shared.c
	$(CC) $(CFLAGS) -O -c chou_asn01_shared.c

chou_asn01_client.o: chou_asn01_client.c
	$(CC) $(CFLAGS) -O -c chou_asn01_client.c

chou_asn01_server.0: chou_asn01_server.c
	$(CC) $(CFLAGS) -O -c chou_asn01_server.c