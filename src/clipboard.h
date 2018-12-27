#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>

char *
clipboard_get_selection_content(Display *display, XFixesSelectionNotifyEvent *event, Atom format);

void
clipboard_init(Display *display);

#endif /* CLIPBOARD_H */
