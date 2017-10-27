#pragma once

#include "../../src/blepo.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
#include <vector>

using namespace blepo; 
using namespace std;

class PointDepth {
public:
	float x,y,z;
	int color;
};


ImgInt disparity_Map(ImgGray& left,ImgGray& right,int min, int max);
void PrintPly(ImgInt in, ImgBgr orig, int &depth);

#define DEPTH_CONST 0.1f




