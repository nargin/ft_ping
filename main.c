#include "header.h"

// default: count = 3, preload = 0, interval = 1s, verbose|quiet|numeric = false
struct flags options = {3, 0, 1, false, false, false};

void	arguments_parser(int ac, char *av[], struct flags *flags) {
	if (ac == 1) {
		print_help(*av);
		return;
	}

	int c;
	while ((c = getopt(ac, av, "vh?c:i:qnl:")) != -1) {
		switch (c) {
			case 'v':
				flags->v = true;
				break;
			case 'h':
				print_help(*av);
			case '?':
				print_help(*av);
			case 'c':
				flags->count = atoi(optarg);
				break;
			case 'i':
				flags->interval = atof(optarg);
				break;
			case 'q':
				flags->q = true;
				break;
			case 'n':
				flags->n = true;
				break;
			case 'l':
				flags->preload = atoi(optarg);
				break;
			default:
				return;
		}
	}

	if (flags->count < 0) {
		printf("ft_ping: invalid argument: count value: out of range: set back to default\n");
		flags->count = 0;
	}

	if (flags->interval < 0.002L) {
		fprintf(stderr, "ft_ping: cannot flood; minimal interval allowed for user is 2ms");
		exit(1);
	}

	if (flags->preload < 0 || flags->preload > 3) {
		char *preload3 = "ft_ping: cannot set preload to value greater than 3: %d\n";
		char *preloadm0 = "ft_ping: invalid argument: '%d': out of range: 1 <= value <= 65536\n";

		fprintf(
			stderr,
			flags->preload < 3 ? preloadm0 : preload3,
			flags->preload
		);
		exit(1);
	}
}

int main(int ac, char *av[]) {
#if __STDC_VERSION__ < 199901L
	# error No for loop
#endif

	if (getuid() != 0) {
		fprintf(stderr, "You must be root to run this program.\n");
		return 1;
	}

	char *hostname = av[ac - 1];
	arguments_parser(ac, av, &options);
	// print_flags(&options);
	
	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP); // AF_INET = IPv4, SOCK_RAW = raw socket, IPPROTO_ICMP = ICMP
	if (sockfd < 0) {
		perror("socket");
		return 1;
	}

	int ttl_val = 64;
	if (setsockopt(sockfd, SOL_IP, IP_TTL, &ttl_val, sizeof(ttl_val)) != 0) {
		printf("\nSetting socket options to TTL failed!\n");
        return 1;
    } else if (options.v) {
        printf("\nSocket set to TTL...\n");
    }

	struct sockaddr_in addr_con;
	char *ip_addr = dns_lookup(hostname, &addr_con);
	if (ip_addr == NULL && options.v) {
		printf("DNS lookup failed! Could not resolve hostname!\n");
	} else if (options.v) {
		printf("Trying to connect to '%s', IP: '%s'\n", hostname, ip_addr);
	}

	if (!ip_addr) {
		printf("%s: %s: name or service not known\n", *av, hostname);
		return 1;
	}

	printf("PING %s (%s)\n", hostname, ip_addr);
	send_ping(sockfd, ip_addr, addr_con);
	
	close(sockfd);
	free(ip_addr);

	return 0;
}