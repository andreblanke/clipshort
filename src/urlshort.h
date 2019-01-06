#ifndef URLSHORT_H
#define URLSHORT_H

#include <stdbool.h>
#include <stddef.h>

#include <curl/curl.h>

#include "string.h"

void
urlshort_free(String *shortened_url);

String *
urlshort_shorten_url(CURL *curl, char *url, size_t url_length, char *hostname);

#endif /* URLSHORT_H */
