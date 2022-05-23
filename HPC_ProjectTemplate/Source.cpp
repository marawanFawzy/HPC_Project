#include <iostream>
#include <math.h>
#include <stdlib.h>
#include<string.h>
#include<msclr\marshal_cppstd.h>
#include <ctime>// include this header 
#include<mpi.h>;
#pragma once

#using <mscorlib.dll>
#using <System.dll>
#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>
using namespace std;
using namespace msclr::interop;
using namespace System::Drawing;
using namespace System;

int* inputImage(int* w, int* h, String^ imagePath) //put the size of image in w & h
{
	int* input;
	int OriginalImageWidth, OriginalImageHeight;
	//*********************************************************Read Image and save it to local arrayss*************************	
	//Read Image and save it to local arrayss
	Bitmap BM(imagePath);
	//cout << BM.Width << endl << BM.Height;
	OriginalImageWidth = BM.Width;
	OriginalImageHeight = BM.Height;
	*w = BM.Width;
	*h = BM.Height;
	int* Red = new int[BM.Height * BM.Width];
	int* Green = new int[BM.Height * BM.Width];
	int* Blue = new int[BM.Height * BM.Width];
	input = new int[BM.Height * BM.Width];
	for (int i = 0; i < BM.Height; i++)
	{
		for (int j = 0; j < BM.Width; j++)
		{
			Color c = BM.GetPixel(j, i);
			Red[i * BM.Width + j] = c.R;
			Blue[i * BM.Width + j] = c.B;
			Green[i * BM.Width + j] = c.G;
			input[i * BM.Width + j] = ((c.R + c.B + c.G) / 3);
		}
	}
	return input;
}
int start_s, stop_s, TotalTime = 0;
void createImage(int* image, int width, int height, int index)
{



	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	Bitmap MyNewImage(width, height);
	//180 // 186
	if (rank == 0)
	{
		for (int i = 1; i < size; i++)
			MPI_Send(image, width * height, MPI_INT, i, 0, MPI_COMM_WORLD);
		int start = rank * ((height * width) / size);
		int end = (rank + 1) * ((height * width) / size);
		for (int k = start; k < end; k++)
		{
			int res = 0, y = k / width , x = k % width , x_mover = -1 , y_mover = -1 , counter = 9;
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					if ((y + y_mover < 0) || (x + x_mover < 0) || (y + y_mover >= MyNewImage.Height) || (x + x_mover >= MyNewImage.Width))
					{
						counter--;
					}
					else res += image[(y + y_mover) * width + (x + x_mover)];
					x_mover++;
				}
				x_mover = -1;
				y_mover++;
			}
			res = res / counter;
			Color c = Color::FromArgb(res, res, res);
			MyNewImage.SetPixel(x, y, c);
		}
		MPI_Status status;
		for (int i = 1; i < size; i++) {
			MPI_Recv(image, height * width, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			int start = i * ((height * width) / size);
			int end = (i + 1) * ((height * width) / size);
			for (int k = start; k < end; k++)
			{
				int y = k / width;
				int x = k % width;
				Color c = Color::FromArgb(image[k], image[k], image[k]);
				MyNewImage.SetPixel(x, y, c);
			}
		}
		MyNewImage.Save("..//Data//Output//outputRes" + index + ".png");
		cout << "result Image Saved " << index << endl;
		stop_s = clock();
		TotalTime += (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000; // seq 3473 
		cout << "time: " << TotalTime << endl;
	}
	else
	{
		int* imageData = new int[height * width];
		MPI_Status status;
		MPI_Recv(imageData, height * width, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		int start = rank * ((height * width) / size);
		int end = (rank + 1) * ((height * width) / size);
		for (int k = start; k < end; k++)
		{
			int res = 0, y = k / width, x = k % width , x_mover = -1 , y_mover = -1 , counter = 9;
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					if ((y + y_mover < 0) || (x + x_mover < 0) || (y + y_mover >= MyNewImage.Height) || (x + x_mover >= MyNewImage.Width))
					{
						counter--;
					}
					else
						res += image[(y + y_mover) * width + (x + x_mover)];
					x_mover++;
				}
				x_mover = -1;
				y_mover++;

			}
			res = res / counter;
			imageData[y * width + x] = res;
		}
		MPI_Send(imageData, width * height, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}

}


int main()
{
	int ImageWidth, ImageHeight;
	String^ imagePath;
	string img;
	img = "..//Data//Input//N.png";
	imagePath = marshal_as<String^>(img);
	int* imageData = inputImage(&ImageWidth, &ImageHeight, imagePath);
	start_s = clock();
	MPI_Init(NULL, NULL);
	for (int i = 0; i < 1; i++)
	{
		MPI_Barrier(MPI_COMM_WORLD);
		createImage(imageData, ImageWidth, ImageHeight, i);
	}
	MPI_Finalize();
	free(imageData);
	return 0;
}



