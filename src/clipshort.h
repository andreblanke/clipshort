#ifndef CLIPSHORT_H
#define CLIPSHORT_H

#include <X11/extensions/Xfixes.h>

#define XFixesSelectionAnyNotifyMask XFixesSetSelectionOwnerNotifyMask      | \
                                     XFixesSelectionWindowDestroyNotifyMask | \
                                     XFixesSelectionClientCloseNotifyMask

#endif /* CLIPSHORT_H */
