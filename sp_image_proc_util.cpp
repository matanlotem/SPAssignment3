#include "sp_image_proc_util.h"
#include <opencv2/imgproc.hpp>//calcHist
#include <opencv2/core.hpp>//Mat
#include <opencv2/highgui.hpp>
#include <opencv2/xfeatures2d.hpp>//SiftDescriptorExtractor
#include <cstdlib>
extern "C" {
	#include "SPBPriorityQueue.h"
}

using namespace cv;

#define IMAGE_LOADING_ERROR "Image cannot be loaded"
#define MEMORY_ERROR "An error occurred - allocation failure\n"

SPPoint* pointFromFloatMat(Mat mat, int i, int dir, int index) {
	// creates an spPoint from the data of a given row/col in a float matrix
	// if dir == 1 takes row i, else takes col i;
	// returns NULL in case of allocation failure

	int dim;
	if (dir)
		dim = mat.cols;
	else
		dim = mat.rows;

	// allocate temporary data array
	double *data = (double*) malloc(dim * sizeof(double));
	if (data == NULL) {
		printf("%s",MEMORY_ERROR);
		return NULL;
	}

	// copy data from mat and convert to double
	if (dir)
		for (int j=0; j<dim; j++)
			data[j] = double(mat.at<float>(i,j));
	else
		for (int j=0; j<dim; j++)
			data[j] = double(mat.at<float>(j,i));

	// create point
	SPPoint* res = spPointCreate(data, dim, index);
	free(data);
	return res;
}

SPPoint** spGetRGBHist(const char* str,int imageIndex, int nBins) {

	Mat src = imread(str,CV_LOAD_IMAGE_COLOR);
	if (src.empty()) {
		printf("%s - %s\n",IMAGE_LOADING_ERROR, str);
		return NULL;
	}

	/// Separate the image in 3 places ( B, G and R )
	std::vector<Mat> bgr_planes;
	split(src, bgr_planes);

	/// Set the ranges ( for B,G,R) )
	float range[] = { 0, 256 };
	const float* histRange = { range };

	// Compute the histograms:
	SPPoint **hist = (SPPoint**) malloc(3*sizeof(SPPoint*));
	if (hist == NULL) {
		printf("%s",MEMORY_ERROR);
		return NULL;
	}

	Mat channel_hist;
	for (int i=0; i<3; i++) {
		// compute histogram
		calcHist(&bgr_planes[i], 1, 0, Mat(), channel_hist, 1, &nBins, &histRange);

		// create spPoint, flip direction to get rgb instead of bgr
		hist[2-i] = pointFromFloatMat(channel_hist, i, 0, imageIndex);

		// in case of allocation failure
		if (hist[2-i] == NULL) {
			printf("%s",MEMORY_ERROR);
			for (int j=0; j<i; j++)
				free(hist[j]);
			free(hist);
			return NULL;
		}
	}

	return hist;
}

double spRGBHistL2Distance(SPPoint** rgbHistA, SPPoint** rgbHistB) {
	if (rgbHistA == NULL || rgbHistB == NULL)
		return -1;

	int i;
	double dist = 0;
	for (i=0; i<3; i++)
		dist += 0.33*spPointL2SquaredDistance(rgbHistA[i],rgbHistB[i]);
	return dist;
}

SPPoint** spGetSiftDescriptors(const char* str, int imageIndex, int nFeaturesToExtract, int *nFeatures) {

	if (str == NULL || nFeatures == NULL || nFeaturesToExtract <= 0)
		return NULL;

	Mat src = imread(str,CV_LOAD_IMAGE_GRAYSCALE);
	if (src.empty()) {
		printf("%s - %s\n",IMAGE_LOADING_ERROR, str);
		return NULL;
	}

	// extract features
	std::vector<cv::KeyPoint> kp1;
	Mat ds1;
	Ptr<xfeatures2d::SiftDescriptorExtractor> detect =
			xfeatures2d::SIFT::create(nFeaturesToExtract);
	detect->detect(src, kp1, Mat());
	detect->compute(src, kp1, ds1);

	// allocate sift descriptors array
	*nFeatures = ds1.rows;
	SPPoint **sift_desc = (SPPoint**) malloc(*nFeatures*sizeof(SPPoint*));
	if (sift_desc == NULL) {
		printf("%s",MEMORY_ERROR);
		return NULL;
	}

	// parse extracted features
	for (int i=0; i<*nFeatures; i++) {
		sift_desc[i] = pointFromFloatMat(ds1 ,i, 1, imageIndex);

		// in case of allocation failure
		if (sift_desc[i] == NULL) {
			printf("%s",MEMORY_ERROR);
			for (int j=0; j<i; j++)
				free(sift_desc[j]);
			free(sift_desc);
			return NULL;
		}
	}

	return sift_desc;
}

int* spBestSIFTL2SquaredDistance(int kClosest, SPPoint* queryFeature,
		SPPoint*** databaseFeatures, int numberOfImages,
		int* nFeaturesPerImage) {

	if (queryFeature == NULL || databaseFeatures == NULL ||
			nFeaturesPerImage == NULL || numberOfImages <= 1)
		return NULL;


	// allocate distance queue
	SPBPQueue* distanceQueue = spBPQueueCreate(kClosest);
	if (distanceQueue == NULL) {
		printf("%s",MEMORY_ERROR);
		return NULL;
	}

	int index;
	double dist;
	// fill  distance queue with indices of kClosest features
	for (int i=0; i<numberOfImages; i++) {
		for (int j=0; j<nFeaturesPerImage[i]; j++) {
			dist = spPointL2SquaredDistance(queryFeature,databaseFeatures[i][j]);
			index = spPointGetIndex(databaseFeatures[i][j]);
			spBPQueueEnqueue(distanceQueue, index, dist);
		}
	}

	// allocate array of closest image indices
	int *closest = (int*) malloc(kClosest * sizeof(int));
	if (closest == NULL) {
		printf("%s",MEMORY_ERROR);
		spBPQueueDestroy(distanceQueue);
		return NULL;
	}

	// populate array of closest image indices
	BPQueueElement elem;
	for (int i=0; i<kClosest; i++) { // assume distanceQueue is full
		spBPQueuePeek(distanceQueue, &elem);
		closest[i] = elem.index;
		spBPQueueDequeue(distanceQueue);
	}
	spBPQueueDestroy(distanceQueue);

	return closest;
}
