#include "urlshort.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

#include "string.h"

inline void
urlshort_free(String *shortened_url)
{
    free(shortened_url->content);
    free(shortened_url);
}

static inline bool
urlshort_is_shortened(char *hostname)
{
    return strstr(hostname, "tinyurl.com") != NULL;
}

static size_t
urlshort_write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t  real_size     = (size * nmemb);
    String *shortened_url = (String *) userp;
    
    shortened_url->content = realloc(
        shortened_url->content,
        shortened_url->length + real_size + 1
    );
    if (!shortened_url) {
        fprintf(stderr, "E: Out of memory (unable to reallocate memory).");
        
        return 0;
    }
    memcpy(
        &(shortened_url->content[shortened_url->length]),
        contents,
        real_size
    );
    
    shortened_url->length                        += real_size;
    shortened_url->content[shortened_url->length] = 0;
    
    return real_size;
}

String *
urlshort_shorten_url(CURL *curl, char *url, size_t url_length, char *hostname)
{
    static const char *SHORTENER_URL = "http://tinyurl.com/api-create.php?url=";
    
    String *shortened_url = malloc(sizeof(String));
    char   *request_url[sizeof(SHORTENER_URL) + url_length];
    
    if (urlshort_is_shortened(hostname)) {
        shortened_url->content = url;
        shortened_url->length  = url_length;
        
        return shortened_url;
    }
    strcpy(request_url, SHORTENER_URL);
    strcat(request_url, url);
    
    shortened_url->length  = 0;
    shortened_url->content = malloc(shortened_url->length + 1);
    
    curl_easy_setopt(curl, CURLOPT_NOBODY,        false);
    curl_easy_setopt(curl, CURLOPT_URL,           request_url);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA,     (void *) shortened_url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &urlshort_write_callback);
    
    if (curl_easy_perform(curl) != CURLE_OK) {
        urlshort_free(shortened_url);
        
        return NULL;
    }
    return shortened_url;
}
