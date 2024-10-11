// source: https://www.geeksforgeeks.org/ping-in-c/

#include "header.h"

// default: count = 3, interval = 1s, verbose|quiet|numeric|preload = false
struct flags options = {3, 1, false, false, false, false};

void print_help(const char *program_name) {
	printf("Usage: %s [OPTIONS] destination\n", program_name);
	printf("Options:\n");
	printf("  -v            Verbose output\n");
	printf("  -h            Display this help message and exit\n");
	printf("  -c count      Stop after sending (and receiving) count ECHO_REQUEST packets\n");
	printf("  -q            Quiet output\n");
	printf("  -i interval   Wait interval seconds between sending each packet\n");
	printf("  -n            Numeric output only, no name resolution\n");
	printf("  -l            Preload (send) count packets before starting normal ping\n");
}

void print_flags(struct flags *flags) {
    printf("Flags:\n");
    printf("  Verbose: %s\n", flags->v ? "true" : "false");
    printf("  Count: %i\n", flags->count);
    printf("  Quiet: %s\n", flags->q ? "true" : "false");
    printf("  Interval: %f\n", flags->interval);
    printf("  Numeric: %s\n", flags->n ? "true" : "false");
    printf("  Preload: %s\n", flags->l ? "true" : "false");
}

void	arguments_parser(int ac, char *av[], struct flags *flags) {
	if (ac == 1) {
		print_help(*av);
		return;
	}

	int c;
	while ((c = getopt(ac, av, "vhc:i:qnl")) != -1) {
		switch (c) {
			case 'v':
				flags->v = true;
				break;
			case 'h':
				print_help(*av);
				break;
			case 'c':
				flags->count = atoi(optarg);
				break;
			case 'q':
				flags->q = true;
				break;
			case 'n':
				flags->n = true;
				break;
			case 'l':
				flags->l = true;
				break;
			default:
				abort();
		}
	}
}

int main(int ac, char *av[]) {
	if (getuid() != 0) {
		fprintf(stderr, "You must be root to run this program.\n");
		return 1;
	}

	arguments_parser(ac, av, &options);
	// print_flags(&options);
	
	int sd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP); // AF_INET = IPv4, SOCK_RAW = raw socket, IPPROTO_ICMP = ICMP
	if (sd < 0) {
		perror("socket");
		return 1;
	}

	int ttl_val = 64;
	if (setsockopt(sd, SOL_IP, IP_TTL, &ttl_val, sizeof(ttl_val)) != 0) {
		printf("\nSetting socket options to TTL failed!\n");
        return 1;
    } else {
        printf("\nSocket set to TTL...\n");
    }

	struct sockaddr_in addr_con;
	char *ip_addr = dns_lookup(av[1], &addr_con);
	if (ip_addr == NULL) {
		printf("DNS lookup failed! Could not resolve hostname!\n");
	} else {
		printf("Trying to connect to '%s', IP: '%s'\n", av[1], ip_addr);
	}

	struct icmphdr icmp;
	memset(&icmp, 0, sizeof(icmp));
	icmp.type = ICMP_ECHO;
	icmp.un.echo.sequence = 0; // 16 bits
	icmp.un.echo.id = getpid() & 0xFFFF; // 16 bits
	icmp.checksum = calculate_checksum((uint16_t *)&icmp, sizeof(icmp));

	// printf("Checksum: %d\n", icmp.checksum);
	// int msg_count = 1;

	while (options.count--) {
		send_packets(sd, icmp, ip_addr, addr_con);
		icmp.un.echo.sequence++;
	}
	
	close(sd);
	free(ip_addr);

	return 0;
}