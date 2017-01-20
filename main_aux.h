/*
 * main_aux.h
 *
 *  Created on: Jan 19, 2017
 *      Author: Matan
 */

#ifndef MAIN_AUX_H_
#define MAIN_AUX_H_

int getUserParams(char *dir, char *prefix, char *suffix,
		int *numOfImages, int *numOfBins, int *nFeaturesToExtract);

int preprocessing(SPPoint ***histDB, SPPoint ***siftDB, int *nFeatures,
		char *dir, char *prefix, char *suffix,
		int numOfImages, int numOfBins, int nFeaturesToExtract);

int queryAndCheck(SPPoint ***histDB, SPPoint ***siftDB, int *nFeatures, int K,
		int numOfImages, int numOfBins, int nFeaturesToExtract);

void destroySPPoint1D(SPPoint **DB, int dim);

void destroySPPoint2D(SPPoint ***DB, int dim, int *nChannels);


#endif /* MAIN_AUX_H_ */
