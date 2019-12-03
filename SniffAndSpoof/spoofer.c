#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <linux/if_ether.h>

uint16_t make_checksum( unsigned short *buf, size_t buflen ) {
	unsigned long sum = 0, i;
	if( buflen < 1 ) return 0;
	for( i=0; i<buflen-1; i+=2 ) {
		sum = sum + (unsigned long)buf[i];
	}
	if( buflen & 1 ) sum += buf[buflen - 1];
	return ~((sum >> 16) + (sum & 0xffff));
}

int ICMP() {
	puts("ICMP Starting");
	int sd;
	struct sockaddr_in sin;
	char buffer[512]; // You can change the buffer size
	memset((void*)&buffer, 0, sizeof(512));
	/* Create a raw socket with IP protocol. The IPPROTO_RAW parameter
	* tells the sytem that the IP header is already included;
	* this prevents the OS from adding another IP header. */
	sd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	if(sd < 0) {
		perror("socket() error"); exit(-1);
	}
	/* This data structure is needed when sending the packets using
	* sockets. Normally, we need to fill out several fields,
	* but for raw sockets, we only need to fill out this one field */
sin.sin_family = AF_INET;

	struct iphdr *ip = (struct iphdr *) buffer;
	struct icmphdr *icmp = (struct icmphdr *) (buffer + sizeof(struct iphdr));

	const char *ip_src = "192.168.15.10";
	const char *ip_dst = "192.168.15.6";
	ip->ihl = 5;
	ip->version = 4;
	ip->tos = 0;
	ip->id = htons(9999);
	ip->frag_off = 0;
	ip->saddr = inet_addr(ip_src);
	ip->daddr = inet_addr(ip_dst);
	ip->tot_len = sizeof(struct iphdr) + sizeof(struct icmphdr);
	ip->ttl = 64;
	ip->protocol = IPPROTO_ICMP;
//	ip->check = 0;
//	ip->check = make_checksum((unsigned short *)ip, sizeof(&ip));
	icmp->type  = ICMP_ECHO;
	printf("chksum: %x\n",make_checksum((unsigned short *)icmp, sizeof(struct icmphdr)));
	printf("chksum: %x\n",make_checksum((unsigned short *)&icmp, sizeof(struct icmphdr)));
	//icmp->checksum = make_checksum((unsigned short *)icmp, sizeof(struct icmphdr));
//	icmp->checksum = htons(make_checksum((unsigned short *)icmp, sizeof(struct icmphdr)));
	htons(make_checksum((unsigned short *)&icmp, sizeof(&icmp)));
	printf("chksum: %x\n",htons(make_checksum((unsigned short *)&icmp, sizeof(&icmp))));
	printf("chksum: %x\n",htons(make_checksum((unsigned short *)icmp, sizeof(&icmp))));
	icmp->checksum = make_checksum((unsigned short *)icmp, sizeof(&icmp));
	// Here you can construct the IP packet using buffer[]
	// - construct the IP header ...
	// - construct the TCP/UDP/ICMP header ...
	// - fill in the data part if needed ...
	// Note: you should pay attention to the network/host byte order.
	/* Send out the IP packet. ip_len is the actual size of the packet. */
	printf("size of ip = %d , size of icmp = %d\n", sizeof(struct iphdr) , sizeof(struct icmphdr));
	if(sendto(sd, buffer, ip->tot_len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0){
		perror("socket()2 error"); exit(-1);
	}
	puts("ICMP Ending");
	return 1;
}

int Ethernet() {
	puts("Ethernet Starting");
	int sd;
	char buffer[512];
	sd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));
	if(sd < 0) {
		perror("Socket() error"); exit(-1);
	}
	struct ifreq source_buf;
	memset(&source_buf, 0x00, sizeof(source_buf));
	strncpy(source_buf.ifr_name, "eth14", IFNAMSIZ);
	ioctl(sd, SIOCGIFINDEX, &source_buf);
	int source_idx = source_buf.ifr_ifindex;

	struct ethhdr *ethernet = (struct ethhdr *) buffer;
	struct iphdr *ip = (struct iphdr *) (buffer + sizeof(struct ethhdr));
	struct icmphdr *icmp = (struct icmphdr *) (buffer + sizeof(struct ethhdr) + sizeof(struct iphdr));


	const unsigned char ethernet_src_addr[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
	const unsigned char ethernet_dst_addr[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	memcpy(ethernet->h_source, ethernet_src_addr,6);
	memcpy(ethernet->h_dest, ethernet_dst_addr,6);
	ethernet->h_proto = htons(ETH_P_IP);



	        const char *ip_src = "192.168.15.10";
        const char *ip_dst = "192.168.15.6";
        ip->ihl = 5;
        ip->version = 4;
        ip->tos = 0;
        ip->id = htons(9999);
        ip->frag_off = 0;
        ip->saddr = inet_addr(ip_src);
        ip->daddr = inet_addr(ip_dst);
        ip->tot_len = sizeof(struct iphdr) + sizeof(struct icmphdr);
        ip->ttl = 64;
        ip->protocol = IPPROTO_ICMP;
	ip->check = htons(0x988d);
	icmp->type  = ICMP_ECHO;
	icmp->checksum = make_checksum((unsigned short *)icmp, sizeof(&icmp));
	       /* const char *ip_src = "192.168.15.10";
        const char *ip_dst = "192.168.15.6";
	ip->ihl = 5;
        ip->version = 4;
//        ip->tos = 0;
//        ip->id = htons(9999);
        ip->frag_off = 0x0;
        ip->saddr = inet_addr(ip_src);
        ip->daddr = inet_addr(ip_dst);
        ip->tot_len = sizeof(struct iphdr) + sizeof(struct icmphdr);
        ip->ttl = 64;
        ip->protocol = IPPROTO_ICMP;
        icmp->type  = ICMP_ECHO;
	//icmp->checksum = make_checksum((unsigned short *)icmp, sizeof(&icmp));
	//icmp->checksum = htons(0x5432);
//	ip->check = make_checksum((unsigned short *)ip, sizeof(&ip));
	printf("chksum: %x\n",make_checksum((unsigned short *)ip, sizeof(struct iphdr)));
	printf("chksum: %x\n",make_checksum((unsigned short *)ip, sizeof(&ip)));
	printf("chksum: %x\n",make_checksum((unsigned short *)ip, 20));
	ip->check = htons(0x9e55);
*/
	struct sockaddr_ll connection;
	memset((void*)&connection, 0, sizeof(connection));
	connection.sll_ifindex = source_idx;
	printf("length = %d\n", ip->tot_len);
	if(sendto(sd, buffer, ip->tot_len+6, 0, (struct sockaddr*)&connection, sizeof(connection)) < 0) {
		perror("socket()2 error"); exit(-1);
	}
}

int main(int argc, char const *argv[]) {
	int num = atoi(argv[1]);
	if(num == 0) {
		printf("Going into ICMP mode\n");
		ICMP();
	}else if(num == 1) {
		printf("Going into Ethernet mode\n");
		Ethernet();
	}
	printf("Done!\n");

}
