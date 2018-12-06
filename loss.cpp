//Include the standard C++ headers  
#include <stdio.h>  
#include <stdlib.h>  
#include <cstdlib>
#include <iostream>

#include "loss.h"

// Arrays should have same size and must >= arrSize!
float pixelLoss(float *arr1, float *arr2, int arrSize) {
	float loss = 0.f;
	for (int i = 0; i < arrSize; i++) {
		float pixelLoss = arr1[i] - arr2[i];
		if (pixelLoss < 0.f) {
			loss -= pixelLoss;
		} else {
			loss += pixelLoss;
		}
	}
	return loss;
}