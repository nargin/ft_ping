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

#define PACKET_SIZE 64

struct flags {
	int	count;
	int	preload;
	double	interval;
	_Bool v;
	_Bool q;
	_Bool n;
};

extern struct flags options;

int send_ping(int sd, char *address, struct sockaddr_in dest_addr);
uint16_t calculate_checksum(uint16_t *addr, int len);
char *dns_lookup(char *addr_host, struct sockaddr_in *addr_con);

void pr_iph(struct iphdr *ip);
void print_flags(struct flags *flags);
void print_help(const char *program_name);

#endif