#include "clipboard.h"

#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>

static Atom XA_INCR;
static Atom XA_XSEL;

void
clipboard_init(Display *display)
{
    XA_INCR = XInternAtom(display, "INCR", false);
    XA_XSEL = XInternAtom(display, "XSEL", false);
}

char *
clipboard_get_selection_content(Display *display, XFixesSelectionNotifyEvent *event, Atom format)
{
    char         *content;

    /* Unused return parameters */
    int           actual_format_return;

    unsigned long nitems_return;
    unsigned long bytes_after_return;

    /* INCR properties are not supported at the moment */
    if (format == XA_INCR) {
        return NULL;
    }

    XConvertSelection(
        display,                    /* display              */
        event->selection,           /* selection            */
        format,                     /* target               */
        XA_XSEL,                    /* property             */
        event->window,              /* requestor            */
        CurrentTime                 /* time                 */
    );

    XGetWindowProperty(
        display,                    /* display              */
        event->window,              /* window               */
        XA_XSEL,                    /* property             */
        0,                          /* long_offset          */
        LONG_MAX / 4,               /* long_length          */
        false,                      /* delete               */
        AnyPropertyType,            /* req_type             */
        &format,                    /* actual_type_return   */
        &actual_format_return,      /* actual_format_return */
        &nitems_return,             /* nitems_return        */
        &bytes_after_return,        /* bytes_after_return   */
        (unsigned char **) &content /* prop_return          */
    );
    return content;
}
