#include <iostream>
#include <sstream>
#include <unistd.h>
#include <vector>

#include <opencv2/imgproc/imgproc.hpp> /* for equalizeHist */
#include <opencv2/opencv.hpp>

#include "face-detect.h"
#include "parrot-image-process.h"

#define DEBUG

struct user_priv {
	cv::CascadeClassifier cascade;
	std::vector<cv::Rect> faces;

	int skip_count;
};

/* path to xml-files */
#define CASCADE_NAME "/haarcascades/haarcascade_frontalface_default.xml"

extern "C" void *face_detect_init()
{
	std::stringstream s;

	if (!getenv("OPENCV_SHARE")) {
		std::cerr << "please set a OPENCV_SHARE-environment variable to help me find my xml-files\n"
		          << "should be something like '/usr/share/opencv'\n";
		return nullptr;
	}

	s << getenv("OPENCV_SHARE")
	  << CASCADE_NAME;

	/* init user-data */
	struct user_priv *p = new user_priv();
	p->cascade = cv::CascadeClassifier(s.str());
	p->skip_count = 0;

#ifdef DEBUG
	cv::namedWindow("Capture");
	cv::namedWindow("Gray");
#endif

	return p;
}

extern "C" void face_detect_work(void *image_data, unsigned int bufsize, GstVideoFormat videoformat, unsigned int width, unsigned int height, void *priv)
{
	struct user_priv *p = reinterpret_cast<struct user_priv *>(priv);

	double scale = width / 320.0; // width of image where the detection will take place

	/* only do facedetction once every X images */
	if (p->skip_count-- <= 0) {
		cv::Mat source;
		int cvt_code;

		switch (videoformat) {
		case GST_VIDEO_FORMAT_YUY2:
			source = cv::Mat(height, width, CV_8UC2, image_data);
			cvt_code = CV_YUV2GRAY_YUYV;
			break;

		case GST_VIDEO_FORMAT_BGR:
			source = cv::Mat(height, width, CV_8UC3, image_data);
			cvt_code = CV_BGR2GRAY;
			break;

		default:
			std::cerr << "unhandled input format\n";
			return;
		}

		cv::Mat gray;

		cv::cvtColor(source, gray, cvt_code);

		/* face detectiong works on very small images -> resize it */
		cv::resize(gray, gray, cv::Size(width / scale, height / scale));
		cv::equalizeHist(gray, gray);

#ifdef DEBUG
		cv::imshow("Gray", gray);
		cv::waitKey(10);
#endif

		p->cascade.detectMultiScale(gray, p->faces, 1.11, 4, 0, cv::Size(40, 40));
		p->skip_count = 4;
	}

	/* output image */
	cv::Mat rect_image;

	enum {
		ONLY_RECT,
		OVERLAY_EVEN_WITH_EXPENSIVE_COPY,
	} mode = OVERLAY_EVEN_WITH_EXPENSIVE_COPY;

	/* create target image */
	switch (mode) {
	case ONLY_RECT:
		rect_image = cv::Mat::zeros(height, width, CV_8UC3);
		break;

	case OVERLAY_EVEN_WITH_EXPENSIVE_COPY:
		switch (videoformat) {
		case GST_VIDEO_FORMAT_YUY2: /* == YUYV */
			cv::cvtColor(cv::Mat(height, width, CV_8UC2, image_data),
			             rect_image,
			             cv::COLOR_YUV2BGR_YUYV); /* conversion and copy */
			break;

		case GST_VIDEO_FORMAT_BGR: /* well BGR is not at all expensive - no copy */
			rect_image = cv::Mat(height, width, CV_8UC3, image_data);
			break;

		case GST_VIDEO_FORMAT_RGB:
			cv::cvtColor(cv::Mat(height, width, CV_8UC3, image_data),
			             rect_image,
			             cv::COLOR_RGB2BGR); /* conversion and copy */
			break;

		default:
			return;
		}
	}

	/* paint rectangle on target image */
	for (auto face_rect : p->faces) {
		/* scale face rectangle to full resolution */
		cv::Rect scaled(face_rect.x * scale, face_rect.y * scale,
		                face_rect.width * scale, face_rect.height * scale);

		//cv::rectangle(orig, scaled, cv::Scalar(255), CV_FILLED);
		cv::rectangle(rect_image, scaled, cv::Scalar(255), 2);
		cv::putText(rect_image, "chelou celui-la", cv::Point(scaled.x, scaled.y - 20),
		            cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);

		//doMosaic(src_img, r->x, r->y, r->width, r->height, 20);
	}

#ifdef DEBUG
	cv::imshow("Capture", rect_image);
	cv::waitKey(10);
#endif

	/* convert back (if necessary) */

	/* create target image */
	switch (mode) {
	case ONLY_RECT:
		/* TODO - blend over or just give back this buffer ? */
		break;

	case OVERLAY_EVEN_WITH_EXPENSIVE_COPY:
		switch (videoformat) {
		case GST_VIDEO_FORMAT_YUY2: /* == YUYV */
			/* TODO manual conversion to YUYV ? */
			//cv::cvtColor(rect_image,
			//			 cv::Mat(height, width, CV_8UC2, image_data),
			//             cv::COLOR_BGR2); /* conversion and copy */
			break;

		case GST_VIDEO_FORMAT_BGR:
			/* nothing to be done ;-) \O/ */
			break;

		case GST_VIDEO_FORMAT_RGB:
			cv::cvtColor(rect_image,
			             cv::Mat(height, width, CV_8UC3, image_data),
			             cv::COLOR_BGR2RGB);
			break;

		default:
			return;
		}
	}
}

extern "C" void face_detect_exit(void *p)
{
	delete reinterpret_cast<struct user_priv *>(p);
}

#if 0
/* functions which pixelizes the faces */
static void doMosaic(IplImage *in, int x0, int y0,
                     int width, int height, int size)
{
	int b, g, r, col, row;

	int xMin = size * (int) floor((double) x0 / size);
	int yMin = size * (int) floor((double) y0 / size);
	int xMax = size * (int) ceil((double) (x0 + width) / size);
	int yMax = size * (int) ceil((double) (y0 + height) / size);

	for (int y = yMin; y < yMax; y += size) {
		for (int x = xMin; x < xMax; x += size) {
			b = g = r = 0;
			for (int i = 0; i < size; i++) {
				if (y + i > in->height) {
					break;
				}
				row = i;
				for (int j = 0; j < size; j++) {
					if (x + j > in->width) {
						break;
					}
					b += (unsigned char) in->imageData[in->widthStep * (y + i) + (x + j) * 3];
					g += (unsigned char) in->imageData[in->widthStep * (y + i) + (x + j) * 3 + 1];
					r += (unsigned char) in->imageData[in->widthStep * (y + i) + (x + j) * 3 + 2];
					col = j;
				}
			}
			row++;
			col++;
			for (int i = 0; i < row; i++) {
				for (int j = 0; j < col; j++) {
					in->imageData[in->widthStep * (y + i) + (x + j) * 3] = cvRound((double) b / (row * col));
					in->imageData[in->widthStep * (y + i) + (x + j) * 3 + 1] = cvRound((double) g / (row * col));
					in->imageData[in->widthStep * (y + i) + (x + j) * 3 + 2] = cvRound((double) r / (row * col));
				}
			}
		}
	}
}
#endif
