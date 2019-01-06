#ifndef NET_H
#define NET_H

#include <stdbool.h>

#include <curl/curl.h>

char *
net_get_hostname(CURLU *curlu, char *url);

CURLcode
net_perform_head_request(CURL *curl, char *url);

#endif /* NET_H */
