#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>

#include <curl/curl.h>

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>

#include "net.h"
#include "x11.h"

#define XFixesSelectionAnyNotifyMask XFixesSetSelectionOwnerNotifyMask      | \
                                     XFixesSelectionWindowDestroyNotifyMask | \
                                     XFixesSelectionClientCloseNotifyMask

/*
 * The following variables should be treated as constants and thus not be
 * modified in any way except by the various init_* procedures which are
 * invoked exactly once from main.
 *
 * They are declared as global static variables to remove clutter from main and
 * avoid having long parameter lists inside the init_* procedures.
 */

/* Xlib */
static Display *display;
static Window   default_root;

static Atom    *standard_selections;
static size_t   standard_selections_count;

/* XFixes */
static int event_base;
static int error_base;

/* Curl */
static CURL  *curl;
static CURLU *curlu;

static noreturn void
clipshort_handle_clipboard_changes()
{
    while (true) {
        char                       *selection_content;
        unsigned long               selection_content_length;
        XFixesSelectionNotifyEvent *selection_event;
        XEvent                      event;

        XNextEvent(display, &event);

        if (event.type != (event_base + XFixesSelectionNotify)) {
            continue;
        }
        selection_event   = (XFixesSelectionNotifyEvent *) &event;
        selection_content = x11_get_selection_content(
            display,
            selection_event,
            XA_STRING,
            &selection_content_length
        );
        if (net_perform_head_request(curl, selection_content) == CURLE_OK) {
            char *hostname = net_get_hostname(curlu, selection_content);

            if (!net_is_shortened(hostname)) {
                char *shortened = net_shorten_url(
                    curl,
                    selection_content,
                    selection_content_length,
                    hostname
                );
            }
            curl_free(hostname);
        }
    }
}

static bool
clipshort_init_xlib()
{
    if ((display = XOpenDisplay(NULL)) == NULL) {
        return false;
    }
    default_root        = DefaultRootWindow(display);
    standard_selections = x11_get_standard_selections(
        display,
        &standard_selections_count
    );
    return true;
}

static bool
clipshort_init_xfixes()
{
    if (!XFixesQueryExtension(display, &event_base, &error_base)) {
        return false;
    }
    for (size_t i = 0; i < standard_selections_count; ++i) {
        XFixesSelectSelectionInput(
            display,
            default_root,
            standard_selections[i],
            XFixesSelectionAnyNotifyMask
        );
    }
    return true;
}

static bool
clipshort_init_curl()
{
    if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK) {
        return false;
    }
    if ((curl = curl_easy_init()) == NULL) {
        return false;
    }
    if ((curlu = curl_url()) == NULL) {
        return false;
    }
    return (curl_easy_setopt(curl, CURLOPT_FAILONERROR, true) == CURLE_OK);
}

int
main(void)
{
    if (!clipshort_init_xlib()) {
        fprintf(stderr, "E: Unable to open display.\n");

        return EXIT_FAILURE;
    }
    if (!clipshort_init_xfixes()) {
        fprintf(stderr, "E: Unable to query XFixes extension.\n");

        return EXIT_FAILURE;
    }
    if (!clipshort_init_curl()) {
        fprintf(stderr, "E: An error occurred initializing libcurl.\n");

        return EXIT_FAILURE;
    }
    clipshort_handle_clipboard_changes();
}
