#ifndef FACE_DETECT_H__
#define FACE_DETECT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <gst/video/video.h>

void *face_detect_init();
void face_detect_work(void *image_data, unsigned int bufsize,
                      GstVideoFormat videoformat, unsigned int width, unsigned int height,
                      void *context);
void face_detect_exit(void *context);

#ifdef __cplusplus
}
#endif

#endif /* FACE_DETECT_H__ */
