#include "header.h"

int	arguments_parser(int ac, char av[]) {
	return 1;
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

int main(int ac, char *av[]) {
	if (getuid() != 0) {
		fprintf(stderr, "You must be root to run this program.\n");
		return 1;
	}
	
	int sd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP); // AF_INET = IPv4, SOCK_RAW = raw socket, IPPROTO_ICMP = ICMP
	if (sd < 0) {
		perror("socket");
		return 1;
	}
	// printf("ICMP sniffa: %d\n", sd);

	char buf[PACKET_SIZE];
	memset(buf, 0, PACKET_SIZE);

	struct icmphdr icmp;
	memset(&icmp, 0, sizeof(icmp));
	icmp.type = ICMP_ECHO;
	icmp.un.echo.sequence = 1; // 16 bits
	icmp.un.echo.id = getpid() & 0xFFFF; // 16 bits
	icmp.checksum = calculate_checksum((uint16_t *)&icmp, sizeof(icmp));

	// printf("Checksum: %d\n", icmp.checksum);

	struct sockaddr_in dest_addr;
	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_addr.s_addr = inet_addr("8.8.8.8");

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

	printf("%d bytes: ", addr_len);
	printf("icmp_seq=%d ", icmp_resp->un.echo.sequence);
	// printf("IP source: %s\t", inet_ntoa(src_addr));
	// printf("ICMP type: %d\n", ((struct icmphdr *)buf)->type);
	printf("ttl= %d ", ip->ttl);
	printf("time=%ld ms\n", rtt);
	
	// printf("Paquet reçu !\n");

	close(sd);
	printf("Adieu monde ! >:)\n");

	return 0;
}