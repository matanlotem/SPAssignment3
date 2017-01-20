/*
 * main.cpp
 *
 *  Created on: Jan 20, 2017
 *      Author: Matan
 */


#include "sp_image_proc_util.h"
#include "main_aux.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>


#define MEMORY_ERROR "An error occurred - allocation failure\n"
#define K 5

/*static int double_compare(const void *a, const void *b) {
	if (*(double*)a > *(double*)b) return 1;
	else if (*(double*)a < *(double*)b) return -1;
	else return 0;
}

int main() {
	printf("start\n");
	int nBins = 16;
	int numOfImages = 16;

	char* imageName = (char*) malloc(1024*sizeof(char));
	double* dists = (double*) malloc(numOfImages*sizeof(double));
	double x;
	SPPoint** im1, **im2;

	im1 = spGetRGBHist("../queryB.png",1,nBins);
	for (int i=0; i<numOfImages; i++) {
		sprintf(imageName,"../images/img%d.png",i);
		im2 = spGetRGBHist(imageName,1,nBins);
		dists[i] = spRGBHistL2Distance(im1, im2);
		printf("%d\t%.0f\n",i,dists[i]);
	}

	qsort(dists,numOfImages,sizeof(double),double_compare);
	printf("\nsorted\n");
	for (int i=0; i<numOfImages; i++)
		printf("%d\t%.0f\n",i,dists[i]);


	im2 = spGetRGBHist("../images/img9.png",1,nBins);
	for (int i=0; i<3; i++) {
		for (int j=0; j<nBins; j++) {
			x = spPointGetAxisCoor(im1[i],j);
			printf("%.0f\t",x);
		}
		printf("\n");
	}
	printf("\n");

	for (int i=0; i<3; i++) {
		for (int j=0; j<nBins; j++) {
			x = spPointGetAxisCoor(im2[i],j);
			printf("%.0f\t",x);
		}
		printf("\n");
	}
	printf("\n");

	printf("dist = %.0f", spRGBHistL2Distance(im1, im2));

	printf("\ndone\n");
	return 0;
}*/

int main () {
	int ret;

	// 1-6. get parameters from user
	int numOfImages, numOfBins, nFeaturesToExtract;
	char *dir = (char*) malloc(1024*sizeof(char));
	char *prefix = (char*) malloc(1024*sizeof(char));
	char *suffix = (char*) malloc(1024*sizeof(char));

	ret = getUserParams(dir, prefix, suffix, &numOfImages, &numOfBins, &nFeaturesToExtract);
	if (ret == -1) { // if input failed (or allocation failure)
		if (dir == NULL || prefix == NULL || suffix == NULL)
			printf("%s",MEMORY_ERROR);
		free(dir);
		free(prefix);
		free(suffix);
		return -1;
	}

	// 7. create databases
	SPPoint ***histDB = (SPPoint***) malloc(numOfImages*sizeof(SPPoint**));
	SPPoint ***siftDB = (SPPoint***) malloc(numOfImages*sizeof(SPPoint**));
	int *nFeatures = (int*) malloc(numOfImages*sizeof(int));
	ret = preprocessing(histDB, siftDB, nFeatures, dir, prefix, suffix, numOfImages, numOfBins, nFeaturesToExtract);
	free(dir);
	free(prefix);
	free(suffix);
	// if preprocessing failed (or allocation failure)
	if (ret == -1) {
		if (histDB == NULL || siftDB == NULL || nFeatures == NULL)
			printf("%s",MEMORY_ERROR);
		destroySPPoint2D(histDB,numOfImages,NULL);
		destroySPPoint2D(siftDB,numOfImages,nFeatures);
		return -1;
	}

	// 8-11. query user
	while (queryAndCheck(histDB, siftDB, nFeatures, K,
			numOfImages, numOfBins, nFeaturesToExtract) == 0) {}

	destroySPPoint2D(histDB,numOfImages,NULL);
	destroySPPoint2D(siftDB,numOfImages,nFeatures);

	return 0;
}

