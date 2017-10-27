//Name: Christopher Cornwall
		

// Homework.cpp : Defines the entry point for the console application.

//
#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <afxwin.h>  // necessary for MFC to work properly
#include "Homework.h"
#include "../../src/blepo.h"
#include <time.h>
#include <math.h>




#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define SQR(x) ((x)*(x))
#define PI 3.14159







using namespace std;
using namespace blepo;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int returnval = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			
			_tprintf(_T("program failed\n"));
			returnval = 1;
		}
		else
		{
			
			ImgBgr img, temp;
			float sigma = 0;

			int compara = InputArgs(argc,argv,sigma,img,temp);
			if(compara == 0) {
				float low_T = 0, high_T = 0;
				ImgFloat float_Img, grad_X, grad_Y, magnitude, phase, edges;
				ImgBinary threshLow, threshHigh;
				 Figure fig("Original Image");
				Figure fig1("X- Gradient");
				Figure fig2("Y- Gradient");
				Figure fig3("Gradient Magnitude");
				 Figure fig4("Gradient Angle");
				  Figure fig5("Egde after Non-Max Suppression");
				 Figure fig6("Canny Edges");
				 Figure fig7("Chamfer Distance");

				 ImgBinary imgCanny, tempCanny;
				ImgInt distance;
				


				fig.Draw(img);
				Convert(img, &float_Img);
				Gradient(float_Img,sigma,grad_X,grad_Y);
				fig1.Draw(grad_X);
				fig2.Draw(grad_Y);

				MagnitudeAndPhase(grad_X, grad_Y, magnitude, phase);
				fig3.Draw(magnitude);
				fig4.Draw(phase);

				NonMaximumSuppression(magnitude,phase,&edges);
				fig5.Draw(edges);

				FindThreshold(edges,low_T,high_T);
				Threshold(edges,threshLow,low_T);
				Threshold(edges,threshHigh,high_T);


				FloodFill(threshLow,threshHigh);
		
				CannyDetect(img,sigma,imgCanny);
				Chamfer(imgCanny,distance);
				fig6.Draw(imgCanny);
				fig7.Draw(distance);
			    } 
			else if
				
				(compara == 1) {
				
				ImgBinary imgCanny, tempCanny;
				ImgInt distance, probMap;

				ImgFloat float_Img, grad_X, grad_Y, magnitude, phase, edges;

				Figure fig("Original Image");
				Figure fig1("X- Gradient");
				Figure fig2("Y- Gradient");
				Figure fig3("Gradient Magnitude");
				 Figure fig4("Gradient Angle");
				  Figure fig5("Egde after Non-Max Suppression");
				  Figure fig6("Canny Edges");
				  Figure fig7("Chamfer Distance");
				 Figure fig8("Canny of Template");
				Figure fig9("Probability Map");
				Figure fig10("Most Likely Position of Template");
				
				

				fig.Draw(img);

				Convert(img, &float_Img);
				Gradient(float_Img,sigma,grad_X,grad_Y);
				fig1.Draw(grad_X);
				fig2.Draw(grad_Y);

				MagnitudeAndPhase(grad_X, grad_Y, magnitude, phase);
				fig3.Draw(magnitude);
				fig4.Draw(phase);

				NonMaximumSuppression(magnitude,phase,&edges);
				fig5.Draw(edges);


				CannyDetect(img,sigma,imgCanny);
				CannyDetect(temp,sigma,tempCanny);
				Chamfer(imgCanny,distance);
				fig6.Draw(imgCanny);
				fig7.Draw(distance);
				fig8.Draw(tempCanny);
				
				
				Rect rect;
				Match(distance,tempCanny,probMap, rect);
				DrawRect(rect,&img,Bgr::RED,1);
				fig9.Draw(probMap);
				fig10.Draw(img);
			} else {
				printf("invalid amount of command-line parameters\n");
			}
			EventLoop();
			cin.get();

		}
	}
	else
	{
		
		_tprintf(_T("Program Fial\n"));
		returnval = 1;
	}

	return returnval;
}

int InputArgs(int argc, TCHAR *argv[], float &sigma, ImgBgr &img, ImgBgr &temp) {
	int tback = -1;
	if(argc != 3 && argc != 4) {
		printf_s("Error: You probably enter too much or too little command-line parameters. This program only take three or four command parameters\n");
		return -1;
	}
	
	CString filname = (argv[2]);
	
	try {
		Load(filname,&img);
		if(img.IsNull())
			throw blepo::Exception();
	} catch(blepo::Exception) {
		printf_s("Error: Could not Load Image\n");
		return -1;
	}
	if(argc == 3) {
		sigma = (float)atof(argv[1]);			
		tback = 0;
	} 
	
	else 
	{
		try {
			//string input2(argv[3]);
			CString template_img = (argv[3]);
			Load(template_img,&temp);
			if(temp.IsNull())
				throw blepo::Exception();
		} catch(blepo::Exception) {
			printf_s("Error: Could not Load Template Image\n");
			return -1;
		}
		sigma = (float)atof(argv[1]);
		tback = 1;
	}
	 // Print command line arguments
        printf("argc = %d\n", argc);
        for (int i=0; i<argc ; i++) 
		{
          printf("argv[%d]=%s\n", i, argv[i]);
		}
		

	return tback;
 

}

void Convolve1D(ImgFloat &in, ImgFloat &kernel, ImgFloat &out) {
	int size, mu, height, width, i, x_cort, y_cort;
	bool horiz;
	assert(kernel.Height() == 1 || kernel.Width() == 1);
	out.Reset(in.Width(),in.Height());
	Set(&out,0.0f);

	if(kernel.Height() == 1) {
		horiz = true;
		size = kernel.Width();
		mu = size / 2;
		height = in.Height();
		width = in.Width() - mu;
		y_cort = 0;
		x_cort = mu;
	} 
	
	else {
		horiz = false;
		size = kernel.Height();
		mu = size / 2;
		height = in.Height() - mu;
		width = in.Width();
		y_cort = mu;
		x_cort = 0;
	}
	ImgFloat::ConstIterator pIn = in.Begin(x_cort,y_cort);
	ImgFloat::ConstIterator pKernel;
	ImgFloat::Iterator pOut = out.Begin(x_cort,y_cort);
	for(int y = y_cort; y < height; y++) {
		for(int x = x_cort; x < width; x++) {
			double val = 0.0f;
			pKernel = kernel.Begin();
			for(int i = 0; i < size; i++) {
				double mult;
				if(horiz)
					mult = *(pIn+mu-i);  
				else
					mult = *(pIn+(in.Width()*(mu-i))); 
				val += (mult * *pKernel++);
			}
			*pOut++ = val;
			pIn++;
		}
	}
}

void Gradient(ImgFloat &img, float sigma, ImgFloat &grad_X, ImgFloat &grad_Y) {
	ImgFloat GX, GY,  GX_d, GY_d, tmp;
	Gaussian(GX, GY, GX_d, GY_d, sigma);
	Convolve1D(img, GX_d, tmp);
	Convolve1D(tmp, GY, grad_X);
	Convolve1D(img, GY_d, tmp);
	Convolve1D(tmp, GX, grad_Y);
}

void Gaussian(ImgFloat &GX, ImgFloat &GY, ImgFloat &GX_d, ImgFloat &GY_d, float sigma) {
	int mu = blepo_ex::Round(2.5f*sigma - 0.5f);
	int width = 2*mu + 1;
	GX.Reset(width,1);
	GY.Reset(1,width);
	GX_d.Reset(width,1);
	GY_d.Reset(1,width);
	float sumGauss = 0, sumDeriv = 0;
	int i = 0;
	ImgFloat::Iterator gX = GX.Begin();
	ImgFloat::Iterator gY = GY.Begin();
	ImgFloat::Iterator gXd = GX_d.Begin();
	ImgFloat::Iterator gYd = GY_d.Begin();
	while(gX != GX.End()) {
		int x = mu - i;
		*gX = exp( -1 * x * x / (2 * sigma * sigma));
		*gY++ = *gX;
		float deriv = x * *gX;
		*gXd = deriv;
		*gYd++ = deriv;
		sumGauss += *gX++;
		sumDeriv += x * *gXd++;
		i++;


	}
	gX = GX.Begin();
	gY = GY.Begin();
	gXd = GX_d.Begin();
	gYd = GY_d.Begin();
	while(gX != GX.End()) {
		*gX++ /= sumGauss;
		*gY++ /= sumGauss;
		*gXd++ /= sumDeriv;
		*gYd++ /= sumDeriv;
	}
	printf("Gaussian is %d\nGaussian derivative is %d\n", sumGauss, sumDeriv);
}

void MagnitudeAndPhase(ImgFloat &x, ImgFloat &y, ImgFloat &magnitude, ImgFloat &phase) {
	magnitude.Reset(x.Width(),x.Height());
	phase.Reset(x.Width(),x.Height());
	ImgFloat::ConstIterator gX = x.Begin();
	ImgFloat::ConstIterator gY = y.Begin();
	ImgFloat::Iterator gMag = magnitude.Begin();
	ImgFloat::Iterator gPhase = phase.Begin();

	while (gX != x.End())
	{
		*gMag++ = sqrt(*gX * *gX + *gY * *gY);
		*gPhase++ = atan2(*gY++, *gX++);
	}
}

void FindThreshold(ImgFloat &img, float &low_T, float &high_T) {
	vector<float> sortedImg;
	ImgFloat::Iterator p = img.Begin();
	while(p != img.End()) {
		if (*p != 0)  
			sortedImg.push_back(*p);
		p++;
	}

	int n = sortedImg.size();
	if (n > 0) 
	
	{
		sort(sortedImg.begin(), sortedImg.end());

		int index = blepo_ex::Round(PERCENT * n);
		high_T = sortedImg[index];
		low_T = high_T / RATIO;
	}
	else
		low_T= high_T = 0;
}

void Threshold(ImgFloat &in, ImgBinary &out, float thresh) {
	out.Reset(in.Width(),in.Height());
	ImgFloat::ConstIterator pIn = in.Begin();
	ImgBinary::Iterator pOut = out.Begin();
	for(int j = 0; j < in.Height(); j++) {
		for(int i = 0; i < in.Width(); i++) {
			if(*pIn++ < thresh)
				*pOut++ = false;
			else
				*pOut++ = true;
		}
	}
}

void FloodFill(ImgBinary &in, ImgBinary &out) {
	vector<Point> *v = new vector<Point>();
	Point seed;
	int color = 1;
	ImgBinary::ConstIterator pIn = in.Begin();
	ImgBinary::ConstIterator pOut = out.Begin();
	for(int j = 0; j < in.Height(); j++) {
		for(int i = 0; i < in.Width(); i++) {
			seed = Point(i,j);
			if(*pOut++) {
				v->push_back(seed);
				while(!v->empty()) {
					Point temp = v->back();
					v->pop_back();
					FillIfNeeded(v,in,out,temp.x-1,temp.y);
					FillIfNeeded(v,in,out,temp.x+1,temp.y);
					FillIfNeeded(v,in,out,temp.x,temp.y-1);
					FillIfNeeded(v,in,out,temp.x,temp.y+1);
					FillIfNeeded(v,in,out,temp.x-1,temp.y-1);
					FillIfNeeded(v,in,out,temp.x+1,temp.y+1);
					FillIfNeeded(v,in,out,temp.x+1,temp.y-1);
					FillIfNeeded(v,in,out,temp.x-1,temp.y+1);
					FillIfNeeded(v,in,out,temp.x,temp.y);
				}
			}
		}
	}
	delete v;
}


inline void FillIfNeeded(vector<Point> *v, ImgBinary &in, ImgBinary &out, int x, int y) {
	if(x >= 0 && y >= 0 && x < in.Width() && y < in.Height() && in(x,y) && !out(x,y)) {
		v->push_back(Point(x,y));
		out(x,y) = true;
	}
}


inline void GetDirection(float phase, int &dx, int &dy) {
	if (phase < 0)  phase += (float) M_PI;

	if (phase < M_PI / 8) {
		dx = 1;
		dy =  0; 
	} else if (phase < 3* M_PI / 8) {
		dx = 1;
		dy =  1;
	} else if (phase < 5 * M_PI / 8) {
		dx = 0;
		dy =  1;
	} else if (phase < 7 * M_PI / 8) {
		dx = -1;
		dy = 1;
	} else {
		dx = -1;
		dy = 0;
	}
}

void NonMaximumSuppression(const ImgFloat& magnitude, const ImgFloat& phase, ImgFloat* out) {
	assert(IsSameSize(magnitude, phase));
	out->Reset(magnitude.Width(), magnitude.Height());
	Set(out, 0); 

	ImgFloat::Iterator pOut = out->Begin(1,1);
	ImgFloat::ConstIterator gMag = magnitude.Begin(1,1);
	ImgFloat::ConstIterator gPhase = phase.Begin(1,1);
	int dx, dy;
	for (int y = 1 ; y < magnitude.Height()-1 ; y++)
	{
		for (int x = 1 ; x < magnitude.Width()-1 ; x++)
		{
			GetDirection(*gPhase++, dx, dy);
			float val0 = *gMag;
			int sub = magnitude.Width()*dy;
			float val1 = *(gMag+dx+sub);
			float val2 = *(gMag-dx-sub);
			*pOut++ = (val0 >= val1 && val0 >= val2) ? val0 : 0;
			gMag++;
		}
		pOut+=2;
		gMag+=2;
		gPhase+=2;
	}
}

void Chamfer(ImgBinary &img, ImgInt &distance) {
	distance.Reset(img.Width(), img.Height());
	const int max = img.Width() * img.Height() + 1;
	int i, j;
	const int width = img.Width() - 1, height = img.Height() - 1;

	ImgBinary::ConstIterator pImg = img.Begin();
	ImgInt::Iterator pDist = distance.Begin();
	
	for (j=0 ; j<= height ; j++)
	{
		for (i=0 ; i <= width ; i++)
		{
			if (*pImg++)
				*pDist++ = 0;
			else
			{
				int value = max;
				if (j > 0)
					value = blepo_ex::Min( value, *(pDist - distance.Width()) + 1);
				if (i > 0)
					value = blepo_ex::Min( value, *(pDist - 1) + 1);
				*pDist++ = value;
			}
		}
	}
	pImg = img.Begin(width,height);
	pDist = distance.Begin(width,height);
	
	for (j=height ; j>=0 ; j--)
	{
		for (i=width ; i>=0 ; i--)
		{
			if (*pImg--)
				*pDist-- = 0;
			else
			{
        int current = *pDist;
	if (j < height-1)
		current = blepo_ex::Min( current, *(pDist + distance.Width()) + 1);
	if (i < width-1)
					current = blepo_ex::Min( current, *(pDist + 1) + 1);
				*pDist-- = current;
	}
	}
	}
}

void Match(ImgInt &distance, ImgBinary &temp, ImgInt &probMap, Rect &area) {

	printf("PLEASE WAIT FOR THE OTHER IMAGES TO LOAD IT TAKES A LITTLE TIME");
	int min = 1;
	
	probMap.Reset((int(distance.Width() - temp.Width())),(int(distance.Height() - temp.Height())));
	ImgBinary::ConstIterator pTemp;
	ImgInt::Iterator pMap = probMap.Begin();
	ImgInt::ConstIterator pDist = distance.Begin();
	for(int i = 0; i < (int(distance.Height() - temp.Height())); i++) 
	{
		for(int j= 0; j < (int(distance.Width() - temp.Width())); j++) 
		{
			int value = 0;
			pTemp = temp.Begin();
			for(int y = 1; y < temp.Height(); y+=3)
		{
			for(int x = 1; x < temp.Width(); x+=3) 
			{
			if(*pTemp++)
			 {
			   value+= *(pDist+x+distance.Width()*y); 
			}

		}
	}
			pDist++;
			*pMap++ = value;
			if(value < min || min < 0) {
				min = value;
				area.bottom = i;
				area.right = j;
			}
		}
		pDist += temp.Width();

    
	}
	area.top = area.bottom + temp.Height();
	area.left = area.right +temp.Width();

	
}

void CannyDetect(ImgBgr &in, float sigma, ImgBinary &out) {
	assert(sigma > 0);
	ImgFloat float_Img, grad_X, grad_Y, magnitude, phase, edges;
	ImgBinary threshLow;
	float low_T = 0, high_T = 0;
	Convert(in, &float_Img);
	Gradient(float_Img,sigma,grad_X,grad_Y);
	MagnitudeAndPhase(grad_X, grad_Y, magnitude, phase);
	NonMaximumSuppression(magnitude,phase,&edges);
	FindThreshold(edges,low_T,high_T);
	Threshold(edges,threshLow,low_T);
	Threshold(edges,out,high_T);
	FloodFill(threshLow,out);
}

