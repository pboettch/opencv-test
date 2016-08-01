#include <opencv2/imgproc/imgproc.hpp> /* for equalizeHist */
#include <opencv2/opencv.hpp>
#include <vector>

/* TODO */
// #include <parrot/image-pipeline.h>
struct parrot_image_meta {
	size_t width, height;
	int image_format; /* apparently it will be yuv or i420 */
};

//static void doMosaic(IplImage *in, int x0, int y0, int width, int height, int size);

struct user_priv {
	cv::CascadeClassifier cascade;
	std::vector<cv::Rect> faces;

	int skip;
};

/* path to xml-files */
#define CASCADE_NAME "/usr/share/opencv/haarcascades/haarcascade_frontalface_default.xml"

static void user_handle_one_image(void *image_data, const struct parrot_image_meta *info, void *priv)
{
	struct user_priv *p = reinterpret_cast<struct user_priv *>(priv);

	cv::Mat orig(info->height, info->width, info->image_format, image_data);

	// yuv or i420

	/* only do facedetction once every X images */
	if (p->skip-- <= 0) {
		cv::Mat gray;
		cv::cvtColor(orig, gray, CV_BGR2GRAY);
		cv::equalizeHist(gray, gray);

		p->cascade.detectMultiScale(gray, p->faces, 1.11, 4, 0, cv::Size(40, 40));
		p->skip = 4;
	}

	for (auto face : p->faces) {
		cv::rectangle(orig, face, cv::Scalar(255), 2); //, CV_FILLED - thickness);
		cv::putText(orig, "chelou celui-la", cv::Point(face.x, face.y - 20),
		            cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);

		//doMosaic(src_img, r->x, r->y, r->width, r->height, 20);
	}
}

int main(int argc, char **argv)
{
	cv::VideoCapture cap(0); // open the default camera
	if (!cap.isOpened())     // check if we succeeded
		return -1;

	/* init user-data */
	struct user_priv p;
	p.cascade = cv::CascadeClassifier(CASCADE_NAME);

	struct parrot_image_meta meta;

	/* "connect" */
	cv::namedWindow("Capture");

	for (;;) {
		cv::Mat frame;
		cap >> frame; // get a new frame from camera

		meta.width = frame.cols;
		meta.height = frame.rows;
		meta.image_format = frame.type();

		user_handle_one_image(frame.ptr(), &meta, &p);

		cv::imshow("Capture", frame);
		if (cv::waitKey(30) >= 0)
			break;
	}

	return 0;
}

#if 0
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
