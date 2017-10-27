#pragma once

#include "../../src/blepo.h"



using namespace blepo;

void DetectingFeatures(ImgGray Gimage);
void LKTrackingFeatures(ImgGray Gimage, ImgGray Gimage2, int window_size);
float Interpolate(ImgFloat img1,float x1, float y1);


#define dist 8


using namespace std;
int flag;
double epsilon=2;
double gxx, gyy, gxy;
double ex, ey;
double dx, dy;
double det;
float sigma = 0.5;

struct features
{
	double x;
	double y;
	double val;
};

struct features feature[100];
struct features feattemp;