#pragma once

// Feel free to add your own code to this header file.

#include "../../src/blepo.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>

using namespace blepo; 
using namespace std;

#define PERCENT 0.70f
#define RATIO	8
int InputArgs(int argc, TCHAR *argv[], float &sigma, ImgBgr &img, ImgBgr &temp);
void Convolve1D(ImgFloat &in, ImgFloat &kernel, ImgFloat &out);
void Gradient(ImgFloat &img, float sigma, ImgFloat &gradX, ImgFloat &gradY);
void Gaussian(ImgFloat &gaussX, ImgFloat &gaussY, ImgFloat &gaussX_deriv, ImgFloat &gaussY_deriv, float sigma);
void MagnitudeAndPhase(ImgFloat &x, ImgFloat &y, ImgFloat &mag, ImgFloat &phase);
void FindThreshold(ImgFloat &img, float &low, float &high);
void FloodFill(ImgBinary &in, ImgBinary &out);
inline void FillIfNeeded(vector<Point> *v, ImgBinary &in, ImgBinary &out, int x, int y);
inline void GetDirection(float phase, int &dx, int &dy);
void NonMaximumSuppression(const ImgFloat& mag, const ImgFloat& phase, ImgFloat* out);
void Threshold(ImgFloat &in, ImgBinary &out, float thresh);
void Chamfer(ImgBinary &img, ImgInt &distance);
void Match(ImgInt &distance, ImgBinary &temp, ImgInt &map, Rect &area);
void CannyDetect(ImgBgr &in, float sigma, ImgBinary &out);


