#include "header.h"

extern struct flags options;

char *dns_lookup(char *addr_host, struct sockaddr_in *addr_con) {
	// struct hostent *host_entity;
	struct addrinfo *res, hints;
	char *ip = malloc(INET_ADDRSTRLEN * sizeof(char)); // 4 * 3(1-9) + 3(.) + '\0'

	hints.ai_family = AF_INET;
	(void)hints;

	printf("Resolving domain name system...\n");
	
	if (getaddrinfo(addr_host, NULL, NULL, &res) != 0) {
		return NULL;
	}

	addr_con = (struct sockaddr_in *)res->ai_addr;

	inet_ntop(res->ai_family, &(addr_con->sin_addr), ip, INET_ADDRSTRLEN);

	return ip;
}

uint16_t calculate_checksum(uint16_t *addr, int len) {
	int nleft = len;
	int sum = 0;
	uint16_t *w = addr;
	uint16_t answer = 0;

	while (nleft > 1)  {
		sum += *w++;
		nleft -= 2;
	}

	if (nleft == 1) {
		*(unsigned char *)(&answer) = *(unsigned char *)w;
		sum += answer;
	}

	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	answer = ~sum;
	return answer;
}

int send_packets(int sd, struct icmphdr icmp, char *address, struct sockaddr_in dest_addr) {
	char buf[PACKET_SIZE];
	memset(buf, 0, PACKET_SIZE);

	// struct sockaddr_in dest_addr;
	// memset(&dest_addr, 0, sizeof(dest_addr));
	// dest_addr.sin_family = AF_INET;
	dest_addr.sin_addr.s_addr = inet_addr(address);

	memcpy(buf, &icmp, sizeof(icmp));

	struct timeval start, end;
	gettimeofday(&start, NULL);

	if (sendto(sd, buf, sizeof(icmp), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
		perror("sendto");
		return 1;
	}
	// printf("Paquet envoyé !\n");

	struct in_addr src_addr;
	socklen_t addr_len = sizeof(src_addr);
	if (recvfrom(sd, buf, PACKET_SIZE, 0, (struct sockaddr *)&src_addr, &addr_len) < 0) {
		perror("recv");
		return 1;
	}

	gettimeofday(&end, NULL);
	long rtt = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;

	struct iphdr *ip = (struct iphdr *)buf;
	struct icmphdr *icmp_resp = (struct icmphdr *)(buf + (ip->ihl * 4));

	printf("\t");
	printf("%d bytes: ", addr_len);
	printf("icmp_seq=%d ", icmp_resp->un.echo.sequence);
	// printf("IP source: %s\t", inet_ntoa(src_addr));
	// printf("ICMP type: %d\n", ((struct icmphdr *)buf)->type);
	printf("ttl= %d ", ip->ttl);
	printf("time=%ld ms\n", rtt);

	sleep(options.interval);

	return 0;
}