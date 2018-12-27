#include <assert.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include <curl/curl.h>

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>

#include "clipboard.h"

static const long XFIXES_SELECTION_ANY_NOTIFY_MASK =
    XFixesSetSelectionOwnerNotifyMask      |
    XFixesSelectionWindowDestroyNotifyMask |
    XFixesSelectionClientCloseNotifyMask;

static const size_t STANDARD_SELECTIONS_COUNT = 3;

static Atom *
clipshort_get_standard_selections(Display *display)
{
    Atom *selections = malloc(STANDARD_SELECTIONS_COUNT * sizeof(Atom));

    if (!selections) {
        return NULL;
    }

    /*
     * For some reason no macro like XA_CLIPBOARD is defined in X11/XAtoms.h,
     * so we intern it ourselves, requiring a Display pointer.
     */
    selections[0] = XInternAtom(display, "CLIPBOARD", false);

    selections[1] = XA_PRIMARY;
    selections[2] = XA_SECONDARY;

    return selections;
}

static inline CURLcode
clipshort_perform_head_request(CURL *curl, char *url)
{
    curl_easy_setopt(curl, CURLOPT_URL,    url);
    curl_easy_setopt(curl, CURLOPT_NOBODY, true);

    return curl_easy_perform(curl);
}

int
main(void)
{
    /* Xlib */
    Atom    *standard_selections;
    Display *display;
    Window   default_root;

    /* Xfixes */
    int error_base;
    int event_base;

    /* Curl */
    CURL *curl;

    /* Xlib initialization */
    if (!(display = XOpenDisplay(NULL))) {
        return EXIT_FAILURE;
    }
    default_root        = DefaultRootWindow(display);
    standard_selections = clipshort_get_standard_selections(display);

    /* Xfixes initialization */
    assert(XFixesQueryExtension(display, &event_base, &error_base));

    for (size_t i = 0; i < STANDARD_SELECTIONS_COUNT; ++i) {
        XFixesSelectSelectionInput(display, default_root, standard_selections[i], XFIXES_SELECTION_ANY_NOTIFY_MASK);
    }

    /* Curl initialization */
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);

    clipboard_init(display);

    /* Listening for clipboard changes */
    while(true) {
        char                       *selection_content;
        XEvent                      event;
        XFixesSelectionNotifyEvent *selection_event;

        XNextEvent(display, &event);

        if (event.type != (event_base + XFixesSelectionNotify)) {
            continue;
        }
        selection_event   = (XFixesSelectionNotifyEvent *) &event;
        selection_content = clipboard_get_selection_content(display, selection_event, XA_STRING);
    }
}
