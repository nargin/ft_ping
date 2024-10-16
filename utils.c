#include "header.h"

void print_help(const char *program_name)
{
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

void print_flags(struct flags *flags)
{
    printf("Flags:\n");
    printf("  Verbose: %s\n", flags->v ? "true" : "false");
    printf("  Count: %i\n", flags->count);
    printf("  Quiet: %s\n", flags->q ? "true" : "false");
    printf("  Interval: %f\n", flags->interval);
    printf("  Numeric: %s\n", flags->n ? "true" : "false");
    printf("  Preload: %s\n", flags->preload ? "true" : "false");
}

void pr_iph(struct iphdr *ip)
{
	// int hlen;
	// u_char *cp;

	// hlen = ip->ihl << 2;
	// cp = (u_char *)ip + 20;		/* point to options */

	printf("Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src      Dst Data\n");
	printf(" %1x  %1x  %02x %04x %04x",
	       ip->version, ip->ihl, ip->tos, ip->tot_len, ip->id);
	printf("   %1x %04x", ((ip->frag_off) & 0xe000) >> 13,
	       (ip->frag_off) & 0x1fff);
	printf("  %02x  %02x %04x", ip->ttl, ip->protocol, ip->check);
	printf(" %s ", inet_ntoa(*(struct in_addr *)&ip->saddr));
	printf(" %s ", inet_ntoa(*(struct in_addr *)&ip->daddr));
	printf("\n");
}