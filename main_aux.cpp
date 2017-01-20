/*
 * main_aux.cpp
 *
 *  Created on: Jan 19, 2017
 *      Author: Matan
 */

/*
 * main_aux.cpp
 *
 *  Created on: Jan 19, 2017
 *      Author: Matan
 */

#include "sp_image_proc_util.h"
#include "main_aux.h"
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <cstring>
extern "C" {
	#include "SPBPriorityQueue.h"
}


#define ENTER_IM_DIR_MSG "Enter images directory path:\n"
#define ENTER_IM_PRE_MSG "Enter images prefix:\n"
#define ENTER_IM_NUM_MSG "Enter number of images:\n"
#define ERROR_IM_NUM_MSG "An error occurred - invalid number of images\n"
#define ENTER_IM_SUF_MSG "Enter images suffix:\n"
#define ENTER_NUM_BINS_MSG "Enter number of bins:\n"
#define ERROR_NUM_BINS_MSG "An error occurred - invalid number of bins\n"
#define ENTER_NUM_FEATURES_MSG "Enter number of features:\n"
#define ERROR_NUM_FEATURES_MSG "An error occurred - invalid number of features\n"
#define ENTER_QUERY_MSG "Enter a query image or # to terminate:\n"
#define EXIT_MSG "Exiting...\n"
#define EXIT_CHAR "#"
#define OUTPUT_GLOBAL_MSG "Nearest images using global descriptors:\n"
#define OUTPUT_LOCAL_MSG "Nearest images using local descriptors:\n"
#define MEMORY_ERROR "An error occurred - allocation failure\n"


int getUserStr(char *str, const char *msg) {
	// get string from user
	// print input message "msg"

	if (str == NULL || msg == NULL)
		return -1;
	printf("%s",msg);
	fgets(str, 1024, stdin);
	str[strcspn(str, "\n")] = 0; // remove \n
	return 0;
}

int getUserInt(int *val, const char *msg, const char *err, int min_val, int max_val) {
	// get integer from user between limits
	// print input message "msg"
	// if out of limits print error message "err" and return -1
	// if successful return 0

	char *str = (char*) malloc(1024*sizeof(char));
	if (str == NULL) {
		printf("%s",MEMORY_ERROR);
		return -1;
	}

	printf("%s",msg);
	fgets(str, 1024, stdin);
	*val = atoi(str);
	if (*val < min_val || *val > max_val) {
		printf("%s",err);
		return -1;
	}

	return 0;
}

int getUserParams(char *dir, char *prefix, char *suffix,
		int *numOfImages, int *numOfBins, int *nFeaturesToExtract) {
	// returns 0 if successful
	// otherwise -1
	if (dir == NULL || prefix == NULL || suffix == NULL ||
			numOfImages == NULL || numOfBins == NULL || nFeaturesToExtract == NULL)
		return -1;

	// 1. directory
	getUserStr(dir, ENTER_IM_DIR_MSG);

	// 2. prefix
	getUserStr(prefix, ENTER_IM_PRE_MSG);

	// 3. number of images
	if (getUserInt(numOfImages,ENTER_IM_NUM_MSG,ERROR_IM_NUM_MSG,1,INT_MAX) == -1) return -1;

	// 4. suffix
	getUserStr(suffix, ENTER_IM_SUF_MSG);

	// 5. number of bins
	if (getUserInt(numOfBins,ENTER_NUM_BINS_MSG,ERROR_NUM_BINS_MSG,1,255) == -1) return -1;

	// 6. number of features
	if (getUserInt(nFeaturesToExtract,ENTER_NUM_FEATURES_MSG,ERROR_NUM_FEATURES_MSG,1,INT_MAX) == -1) return -1;

	return 0;
}

int preprocessing(SPPoint ***histDB, SPPoint ***siftDB, int *nFeatures,
		char *dir, char *prefix, char *suffix,
		int numOfImages, int numOfBins, int nFeaturesToExtract) {
	// compute histogram and sift descriptors for all images
	// if fails returns -1, otherwise 0

	if (histDB == NULL || siftDB == NULL || nFeatures == NULL)
		return -1;

	// allocate memory for image name string
	char *imageName = (char*) malloc(1024*sizeof(char));
	if (imageName == NULL) {
		printf("%s",MEMORY_ERROR);
		return -1;
	}

	// compute histogram and sift descriptors
	for (int i=0; i<numOfImages; i++) {
		sprintf(imageName,"%s%s%d%s", dir, prefix, i, suffix);
		printf("%s\n",imageName); // remove this ###

		// get histogram
		histDB[i] = spGetRGBHist(imageName,i,numOfBins);
		if (histDB[i] == NULL) {
			free(imageName);
			return -1;
		}

		// get sift descriptors
		siftDB[i] = spGetSiftDescriptors(imageName,i,nFeaturesToExtract, nFeatures + i);
		if (siftDB[i] == NULL) {
			free(imageName);
			return -1;
		}
	}
	free(imageName);

	return 0;
}

// struct for sortAndPrint
typedef struct sortable_index {
	int index;
	double value;
} sortable_index;

// comparator for sortAndPrint
int compareIndex(const void *A, const void *B) {
	// sortable_index comparator
	// compares by value and uses index as tie breaker
	const sortable_index *a = (sortable_index *)A;
	const sortable_index *b = (sortable_index *)B;
	if (a->value > b->value || (a->value == b->value && a->index > b->index)) return 1;
	return -1;
}

void sortAndPrint(sortable_index *arr, int dim, int k, int order, char *msg) {
	// sorts an array by value and prints k first indices
	// prints msg and then indices
	// if order = -1, flips sorting order
	if (arr == NULL) return;

	if (order == -1) // flip order if -1
		for (int i=0; i<dim; i++) arr[i].value *= -1;
	qsort(arr, dim, sizeof(sortable_index), &compareIndex);

	if (msg != NULL) printf("%s", msg);
	for (int i=0; i<k-1; i++)
		printf("%d, ",arr[i].index);
	printf("%d\n",arr[k-1].index);
}

int queryAndCheck(SPPoint ***histDB, SPPoint ***siftDB, int *nFeatures, int k,
		int numOfImages, int numOfBins, int nFeaturesToExtract) {
	/**
	 * query user for image (or # for exit)
	 * compare image to preprocessed desctiptors and print k closest image indices
	 * compare once for global descriptors (histogram)
	 * and once for local descriptors (sift features)
	 */

	// allocate memory for query string
	char *query = (char*) malloc(1024*sizeof(char));
	if (query == NULL) {
		printf("%s",MEMORY_ERROR);
		return -1;
	}

	// get query and check exit character
	getUserStr(query, ENTER_QUERY_MSG);
	if (strncmp(query, EXIT_CHAR, 1024) == 0) {
		free(query);
		printf("%s", EXIT_MSG);
		return -1;
	}

	// allocate distance array for comparisons
	sortable_index *dists = (sortable_index*) malloc(numOfImages*sizeof(sortable_index));
	if (dists == NULL) {
		printf("%s",MEMORY_ERROR);
		free(query);
		return -1;
	}

	// compare global descriptors
	// get histogram
	SPPoint **qhist = spGetRGBHist(query,numOfImages+1,numOfBins);
	if (qhist == NULL) { // if failed (error messages printed in function)
		free(query);
		free(dists);
		return -1;
	}
	// compare histograms, sort and print output
	for (int i=0; i<numOfImages; i++) {
		dists[i].value = spRGBHistL2Distance(qhist, histDB[i]);
		dists[i].index = i;
	}
	sortAndPrint(dists, numOfImages, k, 1, OUTPUT_GLOBAL_MSG);
	destroySPPoint1D(qhist, 3); // cleanup


	// compare local descriptors
	// get sift features
	int qnFeatures;
	SPPoint **qsift = spGetSiftDescriptors(query,numOfImages+1,nFeaturesToExtract, &qnFeatures);
	if (qsift == NULL) { // if failed (error messages printed in function)
		free(query);
		free(dists);
		return -1;
	}

	// compare sift features
	int* hits;
	for (int i=0; i<numOfImages; i++) {
		dists[i].value = 0;
		dists[i].index = i;
	}

	for (int i=0; i<qnFeatures; i++) {
		hits = spBestSIFTL2SquaredDistance(k, qsift[i], siftDB, numOfImages, nFeatures);
		if (hits == NULL) { // if failed (error messages printed in function)
			free(query);
			free(dists);
			destroySPPoint1D(qsift, qnFeatures);
		}

		// sum hits
		for (int j=0; j<k; j++) dists[hits[j]].value ++;
		//for (int j=0; j<k; j++)
			//dists[spPointGetIndex(siftDB[hits[j]][0])].value ++;
		free(hits);
	}

	// sort and print
	sortAndPrint(dists, numOfImages, k, -1, OUTPUT_LOCAL_MSG);
	destroySPPoint1D(qsift, qnFeatures); // cleanup

	// cleanup
	free(query);
	free(dists);
	return 0;
}

void destroySPPoint1D(SPPoint **DB, int dim) {
	/**
	 * destroy a 1D SPPoint array of size dim
	 * assumes dim is correct
	 */
	if (DB != NULL) {
		for (int i=0; i<dim; i++)
			spPointDestroy(DB[i]);
		free(DB);
	}
}

void destroySPPoint2D(SPPoint ***DB, int dim, int *nChannels) {
	/**
	 * destroy a 2D SPPoint array
	 * the first dimension is dim
	 * the second dimension is given in nChannels
	 * if nChannels is NULL then the second dimension is 3
	 * assumes dimensions to be correct
	 * destroys nChannels as well
	 */

	if (DB != NULL) {
		for (int i=0; i<dim; i++) {
			if (nChannels != NULL)
				destroySPPoint1D(DB[i],nChannels[i]);
			else
				destroySPPoint1D(DB[i],3);
		}
		free(DB);
	}
	free(nChannels);
}




