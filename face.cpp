#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/ml.h>

/* TODO */
// #include <parrot/image-pipeline.h>
struct parrot_image_meta {
	size_t x, y;
	enum {
		RGB_WHATEVER,
	} format;
};

static void doMosaic(IplImage *in, int x0, int y0, int width, int height, int size);

struct user_priv {
	/* private user's algo */
	CvMemStorage *storage;
	CvHaarClassifierCascade *cascade;
	CvSeq *faces;
	int skip;
};

/* path to xml-files */
#define CASCADE_NAME "/usr/share/opencv/haarcascades/haarcascade_frontalface_default.xml"

static void user_handle_one_image(void *image_data, const struct parrot_image_meta *info, void *priv)
{
	struct user_priv *p = reinterpret_cast<struct user_priv *>(priv);
	IplImage *src_img = reinterpret_cast<IplImage *>(image_data),
	         *src_gray = 0;

	if (p->skip-- <= 0) {
		src_gray = cvCreateImage(cvGetSize(src_img), IPL_DEPTH_8U, 1);
		cvClearMemStorage(p->storage);
		cvCvtColor(src_img, src_gray, CV_BGR2GRAY);
		cvEqualizeHist(src_gray, src_gray);

		p->faces = cvHaarDetectObjects(src_gray, p->cascade, p->storage,
		                               1.11, 4, 0, cvSize(40, 40));
		p->skip = 4;
	}
	for (int i = 0; i < (p->faces ? p->faces->total : 0); i++) {
		CvRect *r = (CvRect *) cvGetSeqElem(p->faces, i);
		doMosaic(src_img, r->x, r->y, r->width, r->height, 20);
	}

	cvReleaseImage(&src_gray);
}

int main(int argc, char **argv)
{
	/* init user-data */
	struct user_priv p = {0};
	p.cascade = (CvHaarClassifierCascade *) cvLoad(CASCADE_NAME, 0, 0, 0);
	p.storage = cvCreateMemStorage(0);

	cvNamedWindow("Capture", CV_WINDOW_AUTOSIZE);
	CvCapture *capture = cvCreateCameraCapture(0);
	assert(capture != NULL);

	struct parrot_image_meta meta = {
	    640, 480, parrot_image_meta::RGB_WHATEVER,
	};

	while (1) {
		IplImage *src_img = cvQueryFrame(capture);

		user_handle_one_image(src_img, &meta, &p);

		cvShowImage("Capture", src_img);

		int c = cvWaitKey(2);
		if (c == '\x1b')
			break;
	}

	cvReleaseCapture(&capture);
	cvDestroyWindow("Capture");

	return 0;
}

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
