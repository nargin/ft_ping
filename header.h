#ifndef HEADER_H
#define HEADER_H

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdbool.h>

// typedef unsigned char bool;
#define PACKET_SIZE 64

struct flags {
	int	count;
	double	interval;
	bool v;
	bool q;
	bool n;
	bool l;
};

extern struct flags options;

int send_packets(int sd, struct icmphdr icmp, char *address, struct sockaddr_in dest_addr);
uint16_t calculate_checksum(uint16_t *addr, int len);
char *dns_lookup(char *addr_host, struct sockaddr_in *addr_con);

#endif