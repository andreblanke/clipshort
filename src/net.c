#include "net.h"

#include <stdbool.h>

#include <curl/curl.h>

char *
net_get_hostname(CURLU *curlu, char *url)
{
    char *hostname;
    
    curl_url_set(curlu, CURLUPART_URL,   url,      false);
    curl_url_get(curlu, CURLUPART_HOST, &hostname, false);
    
    return hostname;
}

CURLcode
net_perform_head_request(CURL *curl, char *url)
{
    curl_easy_setopt(curl, CURLOPT_URL,    url);
    curl_easy_setopt(curl, CURLOPT_NOBODY, true);

    return curl_easy_perform(curl);
}
