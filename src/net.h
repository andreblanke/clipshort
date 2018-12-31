#ifndef NET_H
#define NET_H

#include <stdbool.h>

#include <curl/curl.h>

CURLcode
net_perform_head_request(CURL *curl, char *url);

char *
net_get_hostname(CURLU *curlu, char *url);

bool
net_is_shortened(char *hostname);

char *
net_shorten_url(CURL *curl, char *url, unsigned long url_length,
                char *hostname);

#endif /* NET_H */
