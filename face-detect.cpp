#include <opencv2/imgproc/imgproc.hpp> /* for equalizeHist */
#include <opencv2/opencv.hpp>

#include <sstream>
#include <vector>

#include "parrot-image-process.h" /* for parrot_image_meta-struct */
#include "face-detect.h"

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
	struct user_priv *p = new user_priv;
	p->cascade = cv::CascadeClassifier(s.str());

	return p;
}

extern "C" void face_detect_work(void *image_data,
                                 const struct parrot_image_meta *info,
                                 void *priv)
{
	struct user_priv *p = reinterpret_cast<struct user_priv *>(priv);

	cv::Mat orig(info->height, info->width, info->image_format, image_data);

	// yuv or i420

	/* only do facedetction once every X images */
	if (p->skip_count-- <= 0) {
		cv::Mat gray;
		cv::cvtColor(orig, gray, CV_BGR2GRAY);
		cv::equalizeHist(gray, gray);

		p->cascade.detectMultiScale(gray, p->faces, 1.11, 4, 0, cv::Size(40, 40));
		p->skip_count = 4;
	}

	for (auto face : p->faces) {
		cv::rectangle(orig, face, cv::Scalar(255), 2); //, CV_FILLED - thickness);
		cv::putText(orig, "chelou celui-la", cv::Point(face.x, face.y - 20),
		            cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);

		//doMosaic(src_img, r->x, r->y, r->width, r->height, 20);
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
