#include <opencv2/opencv.hpp>

#include "face-detect.h"
#include "parrot-image-process.h"

int main(int argc, char **argv)
{
	cv::VideoCapture cap(0); // open the default camera
	if (!cap.isOpened())     // check if we succeeded
		return -1;

	void *user_context = face_detect_init();
	if (!user_context)
		exit(1);

//	cv::namedWindow("Capture");

	for (;;) {
		cv::Mat frame;
		cap >> frame; // get a new frame from camera

		struct parrot_image_meta meta = {
			.width = static_cast<size_t>(frame.cols),
			.height = static_cast<size_t>(frame.rows),
			.image_format = parrot_image_meta::BGR,
			.buffer_size = frame.elemSize() * frame.total(),
		};

		face_detect_work(frame.ptr(), &meta, user_context);

//		cv::imshow("Capture", frame);
		if (cv::waitKey(30) >= 0)
			break;
	}

	face_detect_exit(user_context);

	return 0;
}
