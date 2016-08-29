#include <iostream>
#include <sstream>
#include <unistd.h>
#include <vector>

#include <opencv2/imgproc/imgproc.hpp> /* for equalizeHist */
#include <opencv2/opencv.hpp>

#include "face-detect.h"
#include "parrot-image-process.h"

//#define DEBUG

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

template <typename T>
void draw_face_rectangles(T *source, unsigned int width, double scale, const std::vector<cv::Rect> &faces)
{
	int W = width / 2;

	/* paint rectangle on target image */
	for (auto face_rect : faces) {
		cv::Rect scaled(face_rect.x * scale, face_rect.y * scale,
		                face_rect.width * scale, face_rect.height * scale);

		/* line thickness is 2px setting Y-values to 0xFF for white */

		/* horizontal lines */
		for (auto w = scaled.x / 2; w < (scaled.x + scaled.width) / 2; w++) {
			/* top */
			int p = scaled.y * W + w;
			source[p].y0 = 255;
			source[p].y1 = 255;
			source[p + W].y0 = 255;
			source[p + W].y1 = 255;

			/* bottom */
			p += scaled.height * W;
			source[p].y0 = 255;
			source[p].y1 = 255;
			source[p + W].y0 = 255;
			source[p + W].y1 = 255;
		}

		/* vertical lines */
		for (auto h = scaled.y; h < (scaled.y + scaled.height); h++) {
			int p = scaled.x / 2 + h * W; // left
			source[p].y0 = 255;
			source[p].y1 = 255;

			p += scaled.width / 2; // right
			source[p].y0 = 255;
			source[p].y1 = 255;
		}
	}
}

extern "C" void face_detect_work(void *image_data, unsigned int bufsize,
                                 GstVideoFormat videoformat, unsigned int width, unsigned int height, void *ctx)
{
	struct user_priv *priv = reinterpret_cast<struct user_priv *>(ctx);
	double scale = width / 400.0; // width of image where the detection will take place

	/* only do facedetction once every X images */
	if (priv->skip_count-- <= 0) {
		cv::Mat source;
		int cvt_code;

		switch (videoformat) {
		case GST_VIDEO_FORMAT_UYVY:
			source = cv::Mat(height, width, CV_8UC2, image_data);
			cvt_code = CV_YUV2GRAY_UYVY;
			break;

		case GST_VIDEO_FORMAT_YUY2:
			source = cv::Mat(height, width, CV_8UC2, image_data);
			cvt_code = CV_YUV2GRAY_YUYV;
			break;

		case GST_VIDEO_FORMAT_I420:
			source = cv::Mat(height, width, CV_8UC2, image_data);
			cvt_code = CV_YUV2GRAY_I420;
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

		priv->cascade.detectMultiScale(gray, priv->faces, 1.11, 4, 0, cv::Size(40, 40));
		priv->skip_count = 4;
	}
	std::cerr << "found " << priv->faces.size() << " faces\n";

	switch (videoformat) {
	case GST_VIDEO_FORMAT_YUY2: {
		struct yuyv {
			uint8_t y0;
			uint8_t u;
			uint8_t y1;
			uint8_t v;
		};
		draw_face_rectangles<>(reinterpret_cast<struct yuyv *>(image_data), width, scale, priv->faces);
	} break;

	case GST_VIDEO_FORMAT_UYVY: {
		struct uyvy {
			uint8_t u;
			uint8_t y0;
			uint8_t v;
			uint8_t y1;
		};
		draw_face_rectangles<>(reinterpret_cast<struct uyvy *>(image_data), width, scale, priv->faces);
	} break;

	case GST_VIDEO_FORMAT_I420: {
		struct i420 {
			uint8_t y0;
			uint8_t y1;
		};
		draw_face_rectangles<>(reinterpret_cast<struct i420 *>(image_data), width, scale, priv->faces);
	} break;

	case GST_VIDEO_FORMAT_BGR: {
		cv::Mat source = cv::Mat(height, width, CV_8UC3, image_data);

		/* paint rectangle on target image */
		for (auto face_rect : priv->faces) {
			/* scale face rectangle to full resolution */
			cv::Rect scaled(face_rect.x * scale, face_rect.y * scale,
			                face_rect.width * scale, face_rect.height * scale);

			//cv::rectangle(orig, scaled, cv::Scalar(255), CV_FILLED);
			cv::rectangle(source, scaled, cv::Scalar(255), 2);
			cv::putText(source, "chelou celui-la", cv::Point(scaled.x, scaled.y - 20),
			            cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);

			//doMosaic(src_img, r->x, r->y, r->width, r->height, 20);
		}
	} break;

	default:
		std::cerr << "unsupported colorspace - bailing out\n";
		return;
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
