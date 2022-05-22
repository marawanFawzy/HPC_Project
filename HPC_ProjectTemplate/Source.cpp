#include <iostream>
#include <math.h>
#include <stdlib.h>
#include<string.h>
#include<msclr\marshal_cppstd.h>
#include <ctime>// include this header 
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
	cout << BM.Width << endl << BM.Height;
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

			input[i * BM.Width + j] = ((c.R + c.B + c.G) / 3); //gray scale value equals the average of RGB values
			//cout << input[i * BM.Width + j] << " ";
		}
		//cout << i << endl;

	}
	return input;
}


void createImage(int* image, int width, int height, int index)
{
	Bitmap MyNewImage(width, height);

	//created at mask of 45*45 .... not 3*3
	for (int i = 43; i < MyNewImage.Height - 43; i++)
	{
		for (int j = 43; j < MyNewImage.Width - 43; j++)
		{
			int res = 0;
			//TODO remember bonus by changing 2025 -> generic 
			for (int k = 0; k < 2025; k++)
			{
				res = res + (image[(((i - 43) + (int)(k / 45)) * width) + ((j - 43) + (k % 45))]);
			}
			res = res / 2025;

			Color c = Color::FromArgb(res, res, res);
			MyNewImage.SetPixel(j, i, c);

		}
	}
	MyNewImage.Save("..//Data//Output//outputRes" + index + ".png");
	cout << endl << "result Image Saved " << index << endl;
}


int main()
{
	int ImageWidth, ImageHeight;
	while (true)
	{
		string var;
		cin >> var;
		if (var == "end")break;
		int start_s, stop_s, TotalTime = 0;

		String^ imagePath;
		string img;
		img = "..//Data//Input//" + var + ".png";

		imagePath = marshal_as<String^>(img);
		int* imageData = inputImage(&ImageWidth, &ImageHeight, imagePath);


		start_s = clock();
		createImage(imageData, ImageWidth, ImageHeight, 0);
		stop_s = clock();
		TotalTime += (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000;
		cout << "time: " << TotalTime << endl;

		free(imageData);
	}
	return 0;

}



