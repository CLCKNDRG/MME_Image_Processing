#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define height 512
#define width 512
#define FILTER_SIZE 3
#define MYSIZE 9

typedef unsigned char BYTE;

int max(int a, int b);
float ComputeMSE(BYTE* a, BYTE* b);
void Apply_Filter(BYTE* MemIn, BYTE* MemOut, float* filter, int filter_size);

int median(float filter[]);
void MedianFilter(BYTE* MemIn, BYTE* MemOut, float* filter, int filter_size);
void MedianOverset();


float avg_filter[FILTER_SIZE * FILTER_SIZE] = {
	1 / (float)(FILTER_SIZE * FILTER_SIZE), 1 / (float)(FILTER_SIZE * FILTER_SIZE), 1 / (float)(FILTER_SIZE * FILTER_SIZE),
	1 / (float)(FILTER_SIZE * FILTER_SIZE), 1 / (float)(FILTER_SIZE * FILTER_SIZE), 1 / (float)(FILTER_SIZE * FILTER_SIZE),
	1 / (float)(FILTER_SIZE * FILTER_SIZE), 1 / (float)(FILTER_SIZE * FILTER_SIZE), 1 / (float)(FILTER_SIZE * FILTER_SIZE)
};

float your_filter[MYSIZE * MYSIZE];
float result[MYSIZE * MYSIZE];
float temp[MYSIZE * MYSIZE];

int main()
{
	FILE* F_I = fopen("noisy_input.raw", "rb");
	FILE* F_T = fopen("target.raw", "rb");
	FILE* F_G = fopen("GRDN_output.raw", "rb");
	FILE* F_A = fopen("avg_filter_output.raw", "wb");
	FILE* F_O = fopen("output.raw", "wb");


	BYTE* MemIn = new BYTE[height * width];
	BYTE* MemTarget = new BYTE[height * width];
	BYTE* MemGRDN = new BYTE[height * width];
	BYTE* MemAVG = new BYTE[height * width];
	BYTE* MemOut = new BYTE[height * width];

	fread(MemIn, sizeof(BYTE), height * width, F_I);
	fread(MemTarget, sizeof(BYTE), height * width, F_T);
	fread(MemGRDN, sizeof(BYTE), height * width, F_G);

	float mse = 0.0f;

	printf("***Result of applying 3*3 avg. filter\n");
	Apply_Filter(MemIn, MemAVG, avg_filter, FILTER_SIZE);
	mse = ComputeMSE(MemTarget, MemAVG);
	printf("	***MSE: %0.4f\n\n", mse);
	fwrite(MemAVG, sizeof(BYTE), height * width, F_A);

	printf("***Result of applying GRDN filter\n");
	// Apply_Filter(MemIn, MemAVG, avg_filter, FILTER_SIZE);
	mse = ComputeMSE(MemTarget, MemGRDN);
	printf("	***MSE: %0.4f\n\n", mse);
	//fwrite(MemAVG, sizeof(BYTE), height * width, F_A);

	printf("***Result of your filter\n");
	MedianFilter(MemIn, MemOut, your_filter, MYSIZE);
	mse = ComputeMSE(MemTarget, MemOut);
	printf("	***MSE: %0.4f\n\n", mse);
	fwrite(MemOut, sizeof(BYTE), height * width, F_O);

	delete MemIn;
	delete MemTarget;
	delete MemGRDN;
	delete MemAVG;
	delete MemOut;

	fclose(F_I);
	fclose(F_T);
	fclose(F_G);
	fclose(F_A);
	fclose(F_O);
	system("pause");

	return 0;
}

int max(int a, int b)
{
	return a >= b ? a : b;
}

float ComputeMSE(BYTE* a, BYTE* b)
{
	float MSE = 0.0f;

	// Compute MSE 
	for (int h = 0; h < height; h++) {
		for (int w = 0; w < width; w++) {
			MSE += powf(abs(a[h * width + w] - b[h * width + w]), 2);
		}
	}
	MSE /= ((height) * (width));

	return MSE;
}

void Apply_Filter(BYTE* MemIn, BYTE* MemOut, float* filter, int filter_size)
{
	int half_filtersize = int(floor(filter_size / (double(2))));
	float accumulated_val = 0.0f;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			accumulated_val = 0;
			for (int k = 0; k < filter_size; k++)
			{
				for (int l = 0; l < filter_size; l++)
				{
					accumulated_val += filter[k * filter_size + l] * float(MemIn[max(0, i + k - half_filtersize) * width + max(0, j + l - half_filtersize)]);
				}
			}
			MemOut[i * width + j] = BYTE(accumulated_val+0.5);
		}
	}
}

/*#####################################*/

int median(float filter[], int filter_size) {
	int temp;
	int k = (filter_size*filter_size)/2;
	for (int i=1; i<(filter_size*filter_size); i++) {
		for (int j=1; j<(filter_size*filter_size)-1; j++) {
			if (filter[j] > filter[j+1]) {
				temp = filter[j];
				filter[j] = filter[j+1];
				filter[j+1] = temp;
			}
		}
	}
	return filter[k];
}

void MedianFilter(BYTE* MemIn, BYTE* MemOut, float* filter, int filter_size) {
	int half_filtersize = int(floor(filter_size / (double(2))));
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			for (int k=0; k<filter_size; k++) {
				for (int l=0; l<filter_size; l++){
					filter[k * filter_size + l] = MemIn[((i + k - half_filtersize) * width) + (j+l-half_filtersize)];
				}
			}
			MedianOverset();
			MemOut[i * width + j] = median(filter, filter_size);
		}
	}
}

void MedianOverset() {
	for (int i=0; i<height; i++) {
		for (int j=0; j<width; j++) {
			if (temp[i*width + j] < 0) {
				temp[i*width + j] = 0;
			} else if (temp[i*width + j] > 255) {
				temp[i*width + j] = 255;
			} else {
				return;
			}
		}
	}
}