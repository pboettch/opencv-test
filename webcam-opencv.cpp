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

void face_detect_work(void *image_data, unsigned int bufsize,
                      GstVideoFormat videoformat, unsigned int width, unsigned int height,
                      void *context);

		face_detect_work(frame.ptr(),
		                 frame.elemSize() * frame.total(),
						 GST_VIDEO_FORMAT_BGR,
		                 static_cast<size_t>(frame.cols),
		                 static_cast<size_t>(frame.rows),
		                 user_context);

		//		cv::imshow("Capture", frame);
		if (cv::waitKey(30) >= 0)
			break;
	}

	face_detect_exit(user_context);

	return 0;
}
