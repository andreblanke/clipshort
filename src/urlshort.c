#include "urlshort.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

bool
urlshort_is_shortened(CURLU *curlu, char *url)
{
    bool  is_shortened;
    char *hostname;

    curl_url_set(curlu, CURLUPART_URL,  url,       0);
    curl_url_get(curlu, CURLUPART_HOST, &hostname, 0);

    is_shortened = (strstr(hostname, "tinyurl.com") == NULL);

    curl_free(hostname);

    return is_shortened;
}
