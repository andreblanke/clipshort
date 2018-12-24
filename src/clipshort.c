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

static const char *CLIPSHORT_EVENT_WINDOW_TITLE = "clipshort_event_window";

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

static inline Window
clipshort_create_event_window(Display *display)
{
    const Window event_window = XCreateSimpleWindow(
        display,                    /* Display      */
        DefaultRootWindow(display), /* Parent       */
        0,                          /* X            */
        0,                          /* Y            */
        1,                          /* Width        */
        1,                          /* Height       */
        0,                          /* Border width */
        0,                          /* Border       */
        0                           /* Background   */
    );
    XStoreName(display, event_window, CLIPSHORT_EVENT_WINDOW_TITLE);

    return event_window;
}

static void
clipshort_init_xfixes(Display *display,
                      Window event_window,
                      Atom *selections,
                      size_t selections_count,
                      int *event_base_return,
                      int *error_base_return)
{
    assert(XFixesQueryExtension(display, event_base_return, error_base_return));

    for (size_t i = 0; i < selections_count; ++i) {
        XFixesSelectSelectionInput(
            display,
            event_window,
            selections[i],
            XFIXES_SELECTION_ANY_NOTIFY_MASK
        );
    }
}

static void
clipshort_init_curl(CURL **curl)
{
    curl_global_init(CURL_GLOBAL_ALL);
    *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);
}

int
main(void)
{
    /* Xlib */
    Atom    *standard_selections;
    Display *display;
    Window   event_window;

    /* Xfixes */
    int error_base;
    int event_base;

    /* Curl */
    CURL *curl;

    if (!(display = XOpenDisplay(NULL))) {
        return EXIT_FAILURE;
    }
    event_window        = clipshort_create_event_window(display);
    standard_selections = clipshort_get_standard_selections(display);

    clipshort_init_xfixes(
        display,
        event_window,
        standard_selections,
        STANDARD_SELECTIONS_COUNT,
        &event_base,
        &error_base
    );
    clipshort_init_curl(&curl);

    /* Listening for clipboard changes */
    while(true) {
        XEvent                      event;
        XFixesSelectionNotifyEvent *selection_event;

        XNextEvent(display, &event);

        if (event.type != (event_base + XFixesSelectionNotify)) {
            continue;
        }
        selection_event = (XFixesSelectionNotifyEvent *) &event;
    }
}
