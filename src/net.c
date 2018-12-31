#include "net.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

static const char *SHORTENED_HOSTNAMES[] = {
    "git.io",
    "tinyurl.com"
};
static const size_t SHORTENED_HOSTNAMES_COUNT =
    sizeof(SHORTENED_HOSTNAMES) / sizeof(SHORTENED_HOSTNAMES[0]);

typedef struct URLshortener {
    char   *url;
    void  (*init)(const struct URLshortener *, CURL *);
    char *(*get_request_url)(const struct URLshortener *, char *url,
                             unsigned long url_length);
} URLshortener;

CURLcode
net_perform_head_request(CURL *curl, char *url)
{
    curl_easy_setopt(curl, CURLOPT_URL,    url);
    curl_easy_setopt(curl, CURLOPT_NOBODY, true);

    return curl_easy_perform(curl);
}

char *
net_get_hostname(CURLU *curlu, char *url)
{
    char *hostname;

    curl_url_set(curlu, CURLUPART_URL,  url,       0);
    curl_url_get(curlu, CURLUPART_HOST, &hostname, 0);

    return hostname;
}

bool
net_is_shortened(char *hostname)
{
    for (size_t i = 0; i < SHORTENED_HOSTNAMES_COUNT; ++i) {
        if (strstr(hostname, SHORTENED_HOSTNAMES[i]) != NULL) {
            return true;
        }
    }
    return false;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
static void
net_default_shortener_init(const URLshortener *shortener, CURL *curl)
{
    curl_easy_setopt(curl, CURLOPT_NOBODY, false);
}
#pragma clang diagnostic pop

static char *
net_default_shortener_get_request_url(const URLshortener *shortener,
                                      char *url,
                                      unsigned long url_length)
{
    size_t request_url_length = (sizeof(shortener->url) + url_length);
    char  *request_url        = malloc(request_url_length * sizeof(char *));

    strcat(request_url, shortener->url);
    strcat(request_url, url);

    return request_url;
}

static const URLshortener *
net_get_url_shortener(char *hostname)
{
    static const URLshortener github_shortener = {
        .url             = "https://git.io?url=",
        .init            = net_default_shortener_init,
        .get_request_url = net_default_shortener_get_request_url
    };
    static const URLshortener default_shortener = {
        .url             = "https://tinyurl.com/api-create.php?url=",
        .init            = net_default_shortener_init,
        .get_request_url = net_default_shortener_get_request_url
    };

    if (strstr(hostname, "github.com") != NULL) {
        return &github_shortener;
    }
    return &default_shortener;
}

char *
net_shorten_url(CURL *curl, char *url, unsigned long url_length, char *hostname)
{
    char               *request_url;
    const URLshortener *shortener;

    shortener = net_get_url_shortener(hostname);
    shortener->init(shortener, curl);
}
