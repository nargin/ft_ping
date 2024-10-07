#ifndef HEADER_H
#define HEADER_H

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

typedef unsigned char bool;
#define PACKET_SIZE 64

struct flags {
	bool v;
	bool h;
	bool c;
	bool q;
	bool i;
	bool n;
	bool l;
};

#endif