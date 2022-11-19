#define _DEFAULT_SOURCE

#include <stdint.h>
#include <sys/time.h>
#include <netdb.h>
#include <string.h>
#include <string.h>
#include <arpa/inet.h>

static const int MAX_HOPS_COUNT = 30;
static const int QUERIES_COUNT = 3;

void send_response(int sockfd, struct in_addr *sin_addr, uint16_t id, uint16_t ttl);
int wait_reply(int sockfd, uint16_t pid, uint8_t ttl, struct timeval *start_time,
                   struct timeval *end_time, int rdns);
