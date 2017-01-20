#ifndef MAIN_AUX_H_
#define MAIN_AUX_H_

/**
 * Get initial program parameters form user:
 * 	- Image naming convention - directory, prefix, suffix
 * 	  for "../images/img1.png" dir = "../images/", prefix = "img", suffix = ".png"
 * 	- Number of images
 * 	- Descriptor parameters - number of bins for histogram and number of sift features
 *
 * all the parametrs hold only return values
 * @param dir - the image directory
 * @param prefix - prefix of image name
 * @param suffix - suffix of image name
 * @param numOfImages - number of images in directory (must be > 0)
 * @param numOfBins - number of bins in histogram (must be > 0 and < 256)
 * @param nFeaturesToExtract - number of sift features to try to extract (must be > 0)
 * @return 0 if succeeds
 * 	and -1 if fails:
 *    - Any of the arguments is NULL
 *    - Parameters with limits exceed limits
 *    - Memory allocation failure
 */
int getUserParams(char *dir, char *prefix, char *suffix,
		int *numOfImages, int *numOfBins, int *nFeaturesToExtract);


/**
 * Compute histograms and sift features for a list of images
 *  - Images are specified by directory, prefix, suffix and index between 0 and numOfImages
 *    for dir = "../images/", prefix = "img", suffix = ".png", numOfImages = 3 we will get:
 *    "../images/img0.png", "../images/img1.png", "../images/img2.png"
 *  - Histogram and sif features paramaters are passed as input
 *
 * return parameters:
 * @param histDB - 1D array of histograms
 *            histDB[i] points to the channel array (of size 3) of image i
 * @param siftDB - 2D array of siftFeatures
 *            siftDB[i][j] is feature j of image i
 * @param nFeatures - number of sift features for each image
 *
 * input parameters:
 * @param dir - the image directory
 * @param prefix - prefix of image name
 * @param suffix - suffix of image name
 * @param numOfImages - number of images in directory (assumed to be > 0)
 * @param numOfBins - number of bins in histogram (assumed to be > 0 and < 256)
 * @param nFeaturesToExtract - number of sift features to try to extract (assumed to be > 0)
 * @return 0 if succeeds
 * 	and -1 if fails:
 *    - Any of the pointer arguments is NULL
 *    - An error occurs during histogram or sift features calculation
 *    	including error in opening image (for wrong path for example)
 *    - Memory allocation failure
 */
int preprocessing(SPPoint ***histDB, SPPoint ***siftDB, int *nFeatures,
		char *dir, char *prefix, char *suffix,
		int numOfImages, int numOfBins, int nFeaturesToExtract);

/*
 * Queries user for action - either image path or # exit character
 * Computes histogram and sift features for query image
 * Compares query image to pre-processed descriptors and prints k closest image indices
 * once for global descriptors (histogram) and once for local descriptors (sift features)
 *
 * @param histDB - 1D array of histograms
 *            histDB[i] points to the channel array (of size 3) of image i
 * @param siftDB - 2D array of siftFeatures
 *            siftDB[i][j] is feature j of image i
 * @param nFeatures - number of sift features for each image
 * @param numOfImages - number of images in directory (assumed to be > 0)
 * @param numOfBins - number of bins in histogram (assumed to be > 0 and < 256)
 * @param nFeaturesToExtract - number of sift features to try to extract (assumed to be > 0)
 *
 * @return 1 if exit character is entered, 0 if succeeds
 * 	and -1 if fails:
 *    - Any of the pointer arguments is NULL
 *    - An error occurs during histogram or sift features calculation
 *    	including error in opening image (for wrong path for example)
 *    - Memory allocation failure

 */
int queryAndCheck(SPPoint ***histDB, SPPoint ***siftDB, int *nFeatures, int K,
		int numOfImages, int numOfBins, int nFeaturesToExtract);

/**
 * Frees memory of a 1D SPPoint array of size dim
 * Assumes dim is the correct dimension of the array
 *
 * @param DB - a 1D SPPoint array
 * @param dim - size of DB
 */
void destroySPPoint1D(SPPoint **DB, int dim);

/**
 * Frees memory of a 2D SPPoint array (a 1D array of size dim of 1D SPPoint arrays)
 * The dimension of array DB[i] is nChannels[i] or by default 3 if nChannels is NULL
 * Assumes given array dimensions are correct
 * Frees nChannels as well
 *
 * @param DB - a 2D SPPoint array
 * @param dim - number of 1D SSPoint arrays in DB
 * @param nChannels - dimension of each 1D SSPoint array in DB
 *                    if NULL then assumed to be 3 for each array
 */
void destroySPPoint2D(SPPoint ***DB, int dim, int *nChannels);


#endif /* MAIN_AUX_H_ */
