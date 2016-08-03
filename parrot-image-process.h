#ifndef PARROT_IMAGE_PROCESS_H__
#define PARROT_IMAGE_PROCESS_H__

struct parrot_image_meta {
	size_t width, height;
	enum {
		YUYV,
		BGR,
		RGB,
	} image_format; /* apparently it will be yuv or i420 */
	size_t buffer_size;
};

#endif /* PARROT_IMAGE_PROCESS_H__ */
