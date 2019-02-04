#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>

#include <curl/curl.h>

#include <libclipboard.h>

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>

#include "net.h"
#include "urlshort.h"

#define XFixesSelectionAnyNotifyMask XFixesSetSelectionOwnerNotifyMask      | \
                                     XFixesSelectionWindowDestroyNotifyMask | \
                                     XFixesSelectionClientCloseNotifyMask

/*
 * The following variables should be treated as constants and thus not be
 * modified in any way except by the various init_* procedures which are
 * invoked exactly once from main.
 *
 * They are declared as global static variables to remove clutter from main and
 * avoid having long parameter lists inside the various init_* procedures.
 */

/* Xlib */
static Display *display;
static Window   default_root;

static Atom     XA_CLIPBOARD;

/* XFixes */
static int event_base;
static int error_base;

/* libcurl */
static CURL  *curl;
static CURLU *curlu;

/* libclipboard */
static clipboard_c *clipboard;

static clipboard_mode
clipshort_atom_to_clipboard_mode(Atom atom)
{
    if (atom == XA_CLIPBOARD) {
        return LCB_CLIPBOARD;
    }
    if (atom == XA_PRIMARY) {
        return LCB_SELECTION;
    }
#ifdef LCB_SELECTION
    if (atom == XA_SECONDARY) {
        return LCB_SECONDARY;
    }
#endif
}

static noreturn void
clipshort_handle_clipboard_changes()
{
    while (true) {
        char                       *selection_content;
        unsigned long               selection_content_length;
        clipboard_mode              clipboard_mode;
        XFixesSelectionNotifyEvent *selection_event;
        XEvent                      event;

        XNextEvent(display, &event);

        if (event.type != (event_base + XFixesSelectionNotify)) {
            continue;
        }
        selection_event   = (XFixesSelectionNotifyEvent *) &event;
        clipboard_mode    = clipshort_atom_to_clipboard_mode(selection_event->selection);
        selection_content = clipboard_text_ex(
            clipboard,
            &selection_content_length,
            clipboard_mode
        );

        if (net_perform_head_request(curl, selection_content) == CURLE_OK) {
            char   *hostname  = net_get_hostname(curlu, selection_content);
            String *shortened = urlshort_shorten_url(
                curl,
                selection_content,
                selection_content_length,
                hostname
            );
            clipboard_set_text_ex(
                clipboard,
                shortened->content,
                shortened->length,
                clipboard_mode
            );

            urlshort_free(shortened);
            curl_free(hostname);
        }
    }
}

int
main(void)
{
    /* Xlib initialization */
    if ((display = XOpenDisplay(NULL)) == NULL) {
        fprintf(stderr, "E: Unable to open display.\n");

        return EXIT_FAILURE;
    }
    default_root = DefaultRootWindow(display);
    XA_CLIPBOARD = XInternAtom(display, "CLIPBOARD", false);

    /* XFixes initialization */
    if (!XFixesQueryExtension(display, &event_base, &error_base)) {
        fprintf(stderr, "E: Unable to query XFixes extension.\n");

        return EXIT_FAILURE;
    }
    XFixesSelectSelectionInput(display, default_root, XA_PRIMARY,   XFixesSelectionAnyNotifyMask);
    XFixesSelectSelectionInput(display, default_root, XA_CLIPBOARD, XFixesSelectionAnyNotifyMask);

    /* libcurl initialization */
    if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK
            || (curl  = curl_easy_init()) == NULL
            || (curlu = curl_url())       == NULL
            || curl_easy_setopt(curl, CURLOPT_FAILONERROR, true) != CURLE_OK) {
        fprintf(stderr, "E: Unable to initialize libcurl.\n");

        return EXIT_FAILURE;
    }

    /* libclipboard initialization */
    if ((clipboard = clipboard_new(NULL)) == NULL) {
        fprintf(stderr, "E: An error occurred initializing libclipboard.\n");
        
        return EXIT_FAILURE;
    }
    clipshort_handle_clipboard_changes();
}
