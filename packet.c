#include "header.h"

int send_packets(int sd, struct icmphdr icmp, char *address) {
	char buf[PACKET_SIZE];
	memset(buf, 0, PACKET_SIZE);

	struct sockaddr_in dest_addr;
	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
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

	printf("%d bytes: ", addr_len);
	printf("icmp_seq=%d ", icmp_resp->un.echo.sequence);
	// printf("IP source: %s\t", inet_ntoa(src_addr));
	// printf("ICMP type: %d\n", ((struct icmphdr *)buf)->type);
	printf("ttl= %d ", ip->ttl);
	printf("time=%ld ms\n", rtt);

	return 0;
}