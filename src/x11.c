#include "x11.h"

#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>

Atom *
x11_get_standard_selections(Display *display,
                            size_t  *standard_selections_count_return)
{
    static const size_t STANDARD_SELECTIONS_COUNT = 3;

    Atom *selections = malloc(STANDARD_SELECTIONS_COUNT * sizeof(Atom));

    if (!selections) {
        return NULL;
    }
    selections[0] = XInternAtom(display, "CLIPBOARD", false);
    selections[1] = XA_PRIMARY;
    selections[2] = XA_SECONDARY;

    *standard_selections_count_return = STANDARD_SELECTIONS_COUNT;

    return selections;
}

char *
x11_get_selection_content(Display *display, XFixesSelectionNotifyEvent *event,
                          Atom format,
                          unsigned long *selection_content_length_return)
{
    static Atom   XA_INCR = (Atom) 0;
    static Atom   XA_XSEL = (Atom) 0;

    char         *content;

    /* Unused return parameters */
    int           actual_format_return;
    unsigned long bytes_after_return;

    /* Assign XA_INCR and XA_XSEL on first invocation */
    if (XA_INCR == (Atom) 0) {
        XA_INCR = XInternAtom(display, "INCR", false);
    }
    if (XA_XSEL == (Atom) 0) {
        XA_XSEL = XInternAtom(display, "XSEL", false);
    }

    /* INCR properties are not supported at the moment */
    if (format == XA_INCR) {
        return NULL;
    }

    XConvertSelection(
        display,                         /* display              */
        event->selection,                /* selection            */
        format,                          /* target               */
        XA_XSEL,                         /* property             */
        event->window,                   /* requestor            */
        CurrentTime                      /* time                 */
    );

    XGetWindowProperty(
        display,                         /* display              */
        event->window,                   /* window               */
        XA_XSEL,                         /* property             */
        0,                               /* long_offset          */
        LONG_MAX / 4,                    /* long_length          */
        false,                           /* delete               */
        AnyPropertyType,                 /* req_type             */
        &format,                         /* actual_type_return   */
        &actual_format_return,           /* actual_format_return */
        selection_content_length_return, /* nitems_return        */
        &bytes_after_return,             /* bytes_after_return   */
        (unsigned char **) &content      /* prop_return          */
    );
    return content;
}
