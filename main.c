#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>

typedef unsigned char bool;
#define PACKET_SIZE 1024

struct flags {
	bool v;
	bool h;
	bool c;
	bool q;
	bool i;
	bool n;
	bool l;
};

struct icmp_header {
	uint8_t type;
	uint8_t code;
	uint16_t checksum;
	uint16_t id;
	uint16_t sequence;
};

int	arguments_parser(int ac, char av[]) {
	return 1;
}

void checksum(void *buf, int len) {
	unsigned short *buffer = buf;
	unsigned int sum = 0;
	unsigned short result;

	for (sum = 0; len > 1; len -= 2) {
		sum += *buffer++;
	}

	if (len == 1) {
		sum += *(unsigned char *)buffer;
	}

	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;
	printf("Checksum: %d\n", result);
}

int main(int ac, char *av[]) {
	if (getuid() != 0) {
		fprintf(stderr, "You must be root to run this program.\n");
		return 1;
	}

	struct flags flags = {0};
	
	if (arguments_parser(ac, *av) == 0) {
		return 0;
	}

	
	printf("ICMP sniffa: %d\n", IPPROTO_ICMP);
	int sd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sd < 0) {
		perror("socket");
		return 1;
	}
	printf("Socket au top !\n");
	char buf[PACKET_SIZE];
	memset(buf, 0, PACKET_SIZE);

	if (setsockopt(sd, IPPROTO_IP, IP_HDRINCL, &(int){1}, sizeof(int)) < 0) {
		perror("setsockopt");
		return 1;
	}
	printf("Option pour le socket au top !\n");

	struct icmp_header *icmp = (struct icmp_header *)buf;
	icmp->type = 8;
	icmp->code = 0;
	icmp->checksum = 0;
	icmp->id = getpid() & 0xFFFF; // 16 bits
	icmp->sequence = 1;

	checksum(buf, PACKET_SIZE);

	struct sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = 0;
	dest_addr.sin_addr.s_addr = inet_addr("8.8.8.8");

	if (sendto(sd, buf, PACKET_SIZE, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
		perror("sendto");
		return 1;
	}

	close(sd);
	printf("Adieu monde ! >:)\n");

	return 0;
}