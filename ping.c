#include "header.h"

extern struct flags options;

char *dns_lookup(char *addr_host, struct sockaddr_in *addr_con) {
	struct addrinfo *res;
	char *ip = malloc(INET_ADDRSTRLEN * sizeof(char)); // 4 * 3(1-9) + 3(.) + '\0'

	if (options.v) {
		printf("Resolving domain name system...\n");
	}

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

int send_ping(int sockfd, char *address, struct sockaddr_in dest_addr) {
    char send_buf[PACKET_SIZE], recv_buf[PACKET_SIZE];
    struct timeval tv_out;
    tv_out.tv_sec = 1;
    tv_out.tv_usec = 0;

    dest_addr.sin_family = AF_INET;
    // dest_addr.sin_addr.s_addr = inet_addr(address);
	inet_aton(address, &dest_addr.sin_addr);
	if (!address) printf("hihi\n");

	int sequence = 1;
    struct icmphdr icmp_sender;
    memset(&icmp_sender, 0, sizeof(icmp_sender));
    icmp_sender.type = ICMP_ECHO;
    icmp_sender.un.echo.sequence = sequence;
    icmp_sender.un.echo.id = getpid();
    
	// stats purpose
	int msg_count = 0;
	int msg_received_count = 0;
	struct timeval during, elapsed;
	gettimeofday(&during, NULL);

    for ( ;options.count; options.count--) {
        icmp_sender.un.echo.sequence = htons(sequence); // convert to network number???
        icmp_sender.checksum = 0;
        icmp_sender.checksum = calculate_checksum((uint16_t *)&icmp_sender, sizeof(icmp_sender));
        memcpy(send_buf, &icmp_sender, sizeof(icmp_sender));

        struct timeval start, end;
        gettimeofday(&start, NULL);

        ssize_t bytes_sent = sendto(sockfd, send_buf, sizeof(icmp_sender), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (bytes_sent < 0) {
            perror("sendto");
            return 1;
        } else {
			msg_count++;
		}
	
        if (options.v) printf("Sent %zd bytes\n", bytes_sent);

        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv_out, sizeof tv_out);

        struct sockaddr_in src_addr;
        socklen_t addr_len = sizeof(src_addr);
        memset(recv_buf, 0, PACKET_SIZE);
        ssize_t bytes_received = recvfrom(sockfd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&src_addr, &addr_len);
        if (bytes_received < 0) {
            if (errno == EWOULDBLOCK) {
                printf("Request timed out\n");
            } else {
                perror("recvfrom");
            }
            continue;
        }

        gettimeofday(&end, NULL);
        long rtt = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;

        struct iphdr *ip = (struct iphdr *)recv_buf;
		if (options.v) pr_iph(ip);
        struct icmphdr *icmp_resp = (struct icmphdr *)(recv_buf + (ip->ihl * 4));

		msg_received_count++;

        if (icmp_resp->un.echo.sequence != icmp_sender.un.echo.sequence && !options.q) {
            printf("Warning: Received sequence number does not match sent sequence number\n");
        }

		if (!options.n || !options.q) {
			printf("\t");
			printf("%zd bytes received: ", bytes_received);
			printf("icmp_seq=%d ", sequence); // inverse htons
			printf("ttl=%d ", ip->ttl);
			printf("time=%ld ms\n", rtt);

			//debug
			// printf("sender hton: %d, ntoh: %d\n", htons(sequence), ntohs(icmp_sender.un.echo.sequence));
			// printf("response hton: %d, ntoh: %d\n", htons(sequence), ntohs(icmp_resp->un.echo.sequence));
		}

		sequence++;
		if (options.preload - options.count >= 0) sleep(options.interval);
    }

	gettimeofday(&elapsed, NULL);
	long total_time = (elapsed.tv_sec - during.tv_sec) * 1000.0 + (elapsed.tv_usec - during.tv_usec) / 1000.0;

	printf("\n==== %s ping statistics ====\n", address);
	printf(
		"%d packets sent, %d packets received, %d%% packet loss, time %ldms",
		msg_count, msg_received_count, (100 - ((msg_received_count / msg_count) * 100)), total_time
	);
	if (options.preload) {
		printf(" pipe %d", options.preload);
	}
	printf("\n");

    return 0;
}