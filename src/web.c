#include "web.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>



char *
web_get_hostname(CURLU *curlu, char *url)
{
    char *hostname;

    curl_url_set(curlu, CURLUPART_URL,  url,       0);
    curl_url_get(curlu, CURLUPART_HOST, &hostname, 0);

    return hostname;
}

bool
web_is_shortened(char *hostname)
{
    /* TODO: use regular expression instead of strstr checks */
    return (strstr(hostname, "git.io")      != NULL)
        || (strstr(hostname, "tinyurl.com") != NULL);
}

char *
web_shorten_url(CURL *curl, char *url, char *hostname)
{
    char *shortened;

    if (strstr(hostname, "github.com") != NULL) {
        char *shortened_url;

        curl_easy_setopt(curl, CURLOPT_URL, "https://git.io");

    } else {
        curl_easy_setopt(curl, CURLOPT_URL, "");
    }
    return shortened;
}

CURLcode
web_perform_head_request(CURL *curl, char *url)
{
    curl_easy_setopt(curl, CURLOPT_URL,    url);
    curl_easy_setopt(curl, CURLOPT_NOBODY, true);

    return curl_easy_perform(curl);
}
