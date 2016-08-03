#ifndef FACE_DETECT_H__
#define FACE_DETECT_H__

#ifdef __cplusplus
extern "C" {
#endif

struct parrot_image_meta;

void *face_detect_init();
void face_detect_work(void *image_data,
                      const struct parrot_image_meta *info,
                      void *context);
void face_detect_exit(void *context);

#ifdef __cplusplus
}
#endif


#endif /* FACE_DETECT_H__ */
