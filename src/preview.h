#ifndef LIBUVC_EXAMPLE_PREVIEW_H
#define LIBUVC_EXAMPLE_PREVIEW_H

#include "libuvc/libuvc.h"

#ifdef __cplusplus
extern "C" {
#endif

int preview_init(const char *window_name);
int preview_frame(const uvc_frame_t *frame);
void preview_close(void);

#ifdef __cplusplus
}
#endif

#endif
