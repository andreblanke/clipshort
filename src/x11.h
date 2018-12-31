#ifndef X11_H
#define X11_H

#include <stddef.h>

#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>

Atom *
x11_get_standard_selections(Display *display,
                            size_t  *standard_selections_count_return);

char *
x11_get_selection_content(Display *display, XFixesSelectionNotifyEvent *event,
                          Atom format,
                          unsigned long *selection_content_length_return);

#endif /* X11_H */
