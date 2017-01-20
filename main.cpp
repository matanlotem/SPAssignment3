#include "sp_image_proc_util.h"
#include "main_aux.h"
#include <cstdio>
#include <cstdlib>

#define MEMORY_ERROR "An error occurred - allocation failure\n"
// number of closest imagess to find
#define K 5

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
	// if pre-processing failed (or allocation failure)
	if (ret == -1) {
		if (histDB == NULL || siftDB == NULL || nFeatures == NULL)
			printf("%s",MEMORY_ERROR);
		destroySPPoint2D(histDB,numOfImages,NULL);
		destroySPPoint2D(siftDB,numOfImages,nFeatures);
		return -1;
	}

	// 8-11. query user and compare given image to all other images
	while (queryAndCheck(histDB, siftDB, nFeatures, K,
			numOfImages, numOfBins, nFeaturesToExtract) == 0) {}

	// cleanup
	destroySPPoint2D(histDB,numOfImages,NULL);
	destroySPPoint2D(siftDB,numOfImages,nFeatures);

	return 0;
}

