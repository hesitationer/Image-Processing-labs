#pragma once

// Feel free to add your own code to this header file.

#include "../../src/blepo.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>

using namespace blepo; 
using namespace std;

int CheckInputArgs(int argc, TCHAR *argv[], ImgBgr &img, int &threshold);
void Chamfer(ImgGray &Gimage3, ImgInt &distance);



