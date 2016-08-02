#ifndef PARROT_IMAGE_PROCESS_H__
#define PARROT_IMAGE_PROCESS_H__

struct parrot_image_meta {
	size_t width, height;
	int image_format; /* apparently it will be yuv or i420 */
};

#endif /* PARROT_IMAGE_PROCESS_H__ */
