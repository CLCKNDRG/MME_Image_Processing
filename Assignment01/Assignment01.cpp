#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define height 256
#define width 256


typedef unsigned char BYTE;
int size_factor = 1;

void Image_Negative(BYTE* MemIn, BYTE* MemOut);
void Image_Mirroring(BYTE* MemIn, BYTE* MemOut);
void Image_Flipping(BYTE* MemIn, BYTE* MemOut);
void Image_Reducing(BYTE* MemIn, BYTE* MemOut, int size_factor);
void Nearest_Neighbor(BYTE* MemIn, BYTE* MemOut, int size_factor);
void Bilinear_Interpolation(BYTE* MemIn, BYTE* MemOut, int size_factor);

int main()
{
	printf(" Press What You Want!!\n");
	printf(" C: Negative\n M: Mirroring\n F: Flipping \n R: Reducing \n N: Nearest_Neighbor_Interpolation \n B: Bilinear_Interpolation\n");
	char key;
	scanf("%c", &key);

	FILE *F_I = fopen("monkey.raw", "rb");
	FILE *F_O = fopen("monkey_out.raw", "wb");

	BYTE* MemIn = new BYTE[height*width];
	BYTE* MemOut;
	fread(MemIn, sizeof(BYTE), height*width, F_I);

	switch (key)
	{
	case 'C': 
		printf("Inverse\n");
		MemOut = new BYTE[height*width];
		Image_Negative(MemIn, MemOut);
		break;
	case 'M':
		printf("Mirroring\n");
		MemOut = new BYTE[height*width];
		Image_Mirroring(MemIn, MemOut);
		break;
	case 'F':
		printf("Flipping\n");
		MemOut = new BYTE[height*width];
		Image_Flipping(MemIn, MemOut);
		break;
	case 'R':
		printf("Reducing\n");
		printf("Press downsampling factor (e.g. 2): ");
		scanf("%d", &size_factor);

		MemOut = new BYTE[height*width*size_factor*size_factor];
		Image_Reducing(MemIn, MemOut, size_factor);
		break;
	case 'N':
		printf("Nearest_Neighbor Interpolation\n");
		printf("Press upsampling factor (e.g. 2): ");
		scanf("%d", &size_factor);

		MemOut = new BYTE[height*width*size_factor*size_factor];
		Nearest_Neighbor(MemIn, MemOut, size_factor);
		break;
	case 'B':
		printf("Bilinear Interpolation\n");
		printf("Press upsampling factor (e.g. 2): ");
		scanf("%d", &size_factor);

		MemOut = new BYTE[height*width*size_factor*size_factor];
		Bilinear_Interpolation(MemIn, MemOut, size_factor);
		break;
	}

	fwrite(MemOut, sizeof(BYTE), height*width*size_factor*size_factor, F_O);
	
	delete MemIn;
	delete MemOut;
	fclose(F_I);
	fclose(F_O);

	return 0;
}

void Image_Negative(BYTE* MemIn, BYTE* MemOut)
{

	for(int i = 0; i < height; i++ )
	{
		for(int j = 0; j < width; j++ )
		{
			MemOut[i*width+j] = 255-MemIn[i*width+j];
		}
	}
}

void Image_Mirroring(BYTE* MemIn, BYTE* MemOut)
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			MemOut[i * j] = MemIn[i * width - j];
		}
	}
	
}

void Image_Flipping(BYTE* MemIn, BYTE* MemOut)
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			MemOut[i * j] = MemIn[height - i * j];
		}
	}
}

void Image_Reducing(BYTE* MemIn, BYTE* MemOut, int size_factor)
{
	for (int i = 0; i < height / size_factor; i++)
	{
		for (int j = 0; j < width / size_factor; j++)
		{
			MemOut[i * j] = MemIn[(i * size_factor) * (j * size_factor)];
		}
	}

}

void Nearest_Neighbor(BYTE* MemIn, BYTE* MemOut, int size_factor)
{
	for (int i = 0; i < height*size_factor; i++)
	{
		for (int j = 0; j < width*size_factor; j++)
		{
			MemOut[i * j] = MemIn[(i/size_factor) * (j/size_factor)];
		}
	}
}

void Bilinear_Interpolation(BYTE* MemIn, BYTE* MemOut, int size_factor)
{
	for (int i = 0; i < height*size_factor; i++)
	{
		for (int j = 0; j < width*size_factor; j++)
		{
			int a = (int)(j / size_factor);
			int b = (int)(i / size_factor);

			double x1 = (double)j / (double)size_factor - (double)a;
			double x2 = 1 - x1;
			double y1 = (double)i / (double)size_factor - (double)b;
			double y2 = 1 - y1;

			double sq1 = x2 * y2;
			double sq2 = x1 * y2;
			double sq3 = x2 * y1;
			double sq4 = x1 * y1;

			MemOut[i * j] = sq1 * MemIn[a, b] + sq2 * MemIn[a+1, b] + sq3 * MemIn[a, b+1] + sq4 * MemIn[a+1, b+1];
		}
	}
}