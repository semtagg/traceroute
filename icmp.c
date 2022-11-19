#include "icmp.h"

#include <stdio.h>
#include <assert.h>
#include <sys/socket.h> 
#include <stdlib.h>
#include <sys/select.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

static uint16_t compute_checksum(uint16_t *icmph, int len) {
    assert(len >= 0);
    uint16_t ret = 0;
    uint32_t sum = 0;
    uint16_t odd_byte;

    while (len > 1) {
        sum += *icmph++;
        len -= 2;
    }

    if (len == 1) {
        *(uint8_t*)(&odd_byte) = * (uint8_t*)icmph;
        sum += odd_byte;
    }

    sum =  (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    ret =  ~sum;
    return ret; 
}

void send_response(int sockfd, struct in_addr *sin_addr, uint16_t id, uint16_t ttl) {
    struct icmphdr icmp_header;
    icmp_header.type = ICMP_ECHO;
    icmp_header.code = 0;
    icmp_header.un.echo.id = id;
    icmp_header.un.echo.sequence = ttl;
    icmp_header.checksum = 0;
    icmp_header.checksum = compute_checksum((uint16_t *)&icmp_header, sizeof(icmp_header));

    struct sockaddr_in recipient;
    bzero(&recipient, sizeof(recipient));
    recipient.sin_family = AF_INET;
    recipient.sin_addr = *sin_addr;

    int setsockopt_ret = setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));
    if (setsockopt_ret != 0) {
        perror("setsockopt error");
    }

    for (int i = 0; i < QUERIES_COUNT; i++) {
        ssize_t bytes_sent = sendto(sockfd, &icmp_header, sizeof(icmp_header), 0,
                                (struct sockaddr *)&recipient, sizeof(recipient));

        if (bytes_sent < 0) {
            perror("sendto error");
        }
    }
}

static int time_passed(int packets_received, struct timeval *current_time,
                       struct timeval *end_time) {
    if (packets_received >= QUERIES_COUNT || timercmp(current_time, end_time, >)) {
        return 1;
    }

    return 0;
}

int wait_reply(int sockfd, uint16_t pid, uint8_t ttl, struct timeval *start_time,
                   struct timeval *end_time, int rdns) {
    printf("%d. ", ttl);
    int packets_received = 0;
    int host_reached = 0;

    struct timeval deltas;
    struct timeval current_time;

    char last_sender_ip_str[20];
    char sender_ip_str[20];

    gettimeofday(&current_time, NULL);
    while (!time_passed(packets_received, &current_time, end_time)) {
        struct sockaddr_in sender;
        socklen_t sender_len = sizeof(sender);
        uint8_t buffer[IP_MAXPACKET];

        fd_set descriptors;
        FD_ZERO(&descriptors);
        FD_SET(sockfd, &descriptors);
        struct timeval tv;
        timersub(end_time, &current_time, &tv);
        int ready = select(sockfd + 1, &descriptors, NULL, NULL, &tv);
        if (ready < 0) {
            perror("select error");
        } if (ready == 0) {
            printf("* * *");
            break;
        }

        ssize_t packet_len = recvfrom(sockfd, buffer, IP_MAXPACKET, 0, (struct sockaddr *)&sender, &sender_len);
        if (packet_len < 0) {
            perror("recvfrom error");
        }

        gettimeofday(&current_time, NULL);

        const char *inet_ntop_ret = inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));
        assert(inet_ntop_ret != NULL);

        struct iphdr *ip_header = (struct iphdr *) buffer;
        ssize_t ip_header_len = 4 * ip_header->ihl;

        struct icmphdr *icmp_ptr = (struct icmphdr *)(buffer + ip_header_len);

        uint8_t icmp_type = icmp_ptr->type;
        int proper_type = icmp_type == ICMP_TIME_EXCEEDED || icmp_type == ICMP_ECHOREPLY;

        if (icmp_type == ICMP_TIME_EXCEEDED) {
            struct iphdr *inner_ip_header = (void *) icmp_ptr + 8;
            ssize_t inner_ip_header_len = 4 * inner_ip_header->ihl;
            icmp_ptr = (void *)inner_ip_header + inner_ip_header_len;
        }

        if (proper_type && icmp_ptr->un.echo.id == pid && icmp_ptr->un.echo.sequence == ttl) {
            timersub(&current_time, start_time, &deltas);

            if (strcmp(last_sender_ip_str, sender_ip_str) != 0) {
                if (rdns) {
                    char hbuf[NI_MAXHOST];
                    if (getnameinfo((struct sockaddr *) &sender, sender_len, hbuf, sizeof(hbuf), NULL, 0, NI_NAMEREQD) != 0) {
                        strcpy(hbuf, sender_ip_str);
                    }
                    printf("%s (%s) ", hbuf, sender_ip_str);
                } else {
                    printf("%s ", sender_ip_str);
                }
            }
            strcpy(last_sender_ip_str, sender_ip_str);
            printf(" %.3f ms ", deltas.tv_usec / 1000.0);

            packets_received++;
            if (icmp_type == ICMP_ECHOREPLY) {
                host_reached = 1;
            }
        }
    }

    printf("\n");
    return host_reached;
}
