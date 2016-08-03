/* this is not-working pseudo code */

#include "parrot-image-process.h"
#include "parrot-pimp-snap-plugin.h"

#include "face-detect.h"

int main()
{
	void *context = face_detect_init();

	if (!context)
		bailout();

	pimp_whatever_connect();

	fd = pimp_get_me_the_dmabuf_fd();

	while (ok) {
		void *p = dma_buffer_magic_from_fd(fd);

		struct parrot_image_meta meta = {
		    .width = 1280,
		    .height = 720,
		    .image_format = YUYV,
		    .buffer_size = size_of_buffer(p),
		};

		/* face-detect now changes the buffer behind p, if it detects some faces */
		face_detect_work(p, &meta, context);
	}

	face_detect_exit(context);
	return 0;
}
