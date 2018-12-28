#ifndef URLSHORT_H
#define URLSHORT_H

#include <stdbool.h>

#include <curl/curl.h>

bool
urlshort_is_shortened(CURLU *curlu, char *url);

#endif /* URLSHORT_H */
