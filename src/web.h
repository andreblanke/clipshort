#ifndef URL_H
#define URL_H

#include <stdbool.h>

#include <curl/curl.h>

char *
web_get_hostname(CURLU *curlu, char *url);

bool
web_is_shortened(char *hostname);

char *
web_shorten_url(CURL *curl, char *url, char *hostname);

CURLcode
web_perform_head_request(CURL *curl, char *url);

#endif /* URL_H */
