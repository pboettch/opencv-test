/*
 * author: Yann Sionneau <yann.sionneau@parrot.com>
 */

#include <face-detect.h>
#include <libmuta.h>
#include <stdlib.h>

int main(void)
{
	void *context = face_detect_init();
	if (!context)
		return EXIT_FAILURE;

	muta_connect(face_detect_work, context);
	face_detect_exit(context);

	return EXIT_SUCCESS;
}
