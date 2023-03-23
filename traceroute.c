#include "icmp.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

int main(int argc, char **argv) {
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: sudo %s [ -n ] host\n", argv[0]);
        return EXIT_FAILURE;
    }

    int rdns = 1;
    if (argc == 3 && strcmp(argv[1], "-n") == 0) {
        argv[1] = argv[2];
        rdns = 0;
    }

    struct hostent *he;
    if ((he = gethostbyname(argv[1])) == NULL) {
        fprintf(stderr, "Host not valid!\nUsage: sudo %s [ -n ] host\n", argv[0]);
        return EXIT_FAILURE;
    }

    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("socket error"); 
        exit(EXIT_FAILURE);
    }

    u_int16_t pid = getpid();
    printf("traceroute to %s (%s), %d hops max\n", he->h_name, inet_ntoa(*(struct in_addr*)he->h_addr_list[0]), MAX_HOPS_COUNT);
    for (int ttl = 1; ttl <= MAX_HOPS_COUNT; ttl++) {
        struct timeval start_time, end_time;
        gettimeofday(&start_time, NULL);
        end_time = start_time;
        end_time.tv_sec++;

        send_response(sockfd, (struct in_addr*)he->h_addr_list[0], pid, ttl);
        int host_reached = wait_reply(sockfd, pid, ttl, &start_time, &end_time, rdns);
        
        if (host_reached) {
            break;
        }
    }

    return EXIT_SUCCESS;
}
