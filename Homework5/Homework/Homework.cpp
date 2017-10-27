//Name: Christopher Cornwall
		

// Homework.cpp : Defines the entry point for the console application.

//
#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <afxwin.h>  
#include "Homework.h"
#include "../../src/blepo.h"
#include <time.h>
#include <math.h>
#include <deque>





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
			try
		{

			if(argc == 4)
			{
			printf("Give it litte time for the images to load\nMax Disparity is 15\n");

			
			CString path = "../../images/";
			CString left_filename = path + (CString) argv[1];
			CString right_filename = path + (CString) argv[2];
			int max_disparity  = atoi(argv[3]);
			int min_disparity  = 0;
			int x,y;
			int depth;
			
			
			ImgBgr img, img2;
			ImgGray Gimage1, Gimage2;
			ImgInt int1,int2, temp;
			
			Figure fig("Left Image");
			Figure fig1("Right Image");
			Figure fig2("Left Disparity Map");
			Figure fig3("Left-Right Consistency Check");

			Load(left_filename, &img);
			Load(right_filename, &img2);
				Convert(img,&Gimage1);
				Convert(img2,&Gimage2);






				
				int sign = -1;

				int1 = disparity_Map(Gimage1,Gimage2,min_disparity,max_disparity);
				temp = disparity_Map(Gimage2,Gimage1,(sign * max_disparity),min_disparity);

				int2=int1;
				for(y=0; y<int1.Height(); y++)
				{
				for(x=0; x<int1.Width(); x++)
				{
					if(x-int1(x,y) > 0)
					{
						if(int1(x,y) != (temp(x-int1(x,y),y)))
							int2(x,y)=0;
					}
				}
				}
				
				
				fig.Draw(Gimage1);
			    fig1.Draw(Gimage2);
				fig2.Draw(int1);
				fig3.Draw(int2);

				
				PrintPly(int2,img,depth);
				 
				 
				
	
			} 
			else 
			{
				printf("invalid amount of command arguments \n");
			}

			}
		catch (Exception& e)
		{
        e.Display();
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







ImgInt disparity_Map(ImgGray& left,ImgGray& right,int min, int max)
{
	ImgGray bar[40];
	ImgInt disparity,dbar[40];
	int x,y,i,d;
	for(i=min; i<max; i++)
	{
		bar[abs(i)]=left;
	}
	for(d=min; d<max; d++)
	{
		for(y=0; y<left.Height(); y++)
		{
			for(x=0; x<left.Width(); x++)
			{
				if(((x-d) >= 0) && ((x-d) <= (left.Width()-1)))
					bar[abs(d)](x,y)=abs(left(x,y)-right(x-d,y));
			}
		}
		Convert(bar[abs(d)],&dbar[abs(d)]);
	}
	ImgInt inter[40],temp,gradx[40];
	for(d=0; d<40; d++)
	{
		inter[d].Reset(left.Width(),left.Height());
		gradx[d].Reset(left.Width(),left.Height());
	}
	temp.Reset(left.Width(),left.Height());

	
	for(d=min; d<max; d++)
	{
		for(y=0; y<left.Height(); y++)
		{
			for(x=2; x<(left.Width()-2); x++)
			{
				if(x==2)
					gradx[abs(d)](2,y)=dbar[abs(d)](0,y)+dbar[abs(d)](1,y)+dbar[abs(d)](2,y)
						+dbar[abs(d)](3,y)+dbar[abs(d)](4,y);
				else if((x+3) < left.Width())
					gradx[abs(d)](x,y)=gradx[abs(d)](x-1,y)-dbar[abs(d)](x-3,y)+dbar[abs(d)](x+2,y);
			}
		}
		for(x=0; x<left.Width(); x++)
		{
			for(y=2; y<(left.Height()-2); y++)
			{
				if(y==2)
					inter[abs(d)](x,2)=gradx[abs(d)](x,0)+gradx[abs(d)](x,1)+gradx[abs(d)](x,2)
						+gradx[abs(d)](x,3)+gradx[abs(d)](x,4);
				else if((y+3) < left.Height())
					inter[abs(d)](x,y)=inter[abs(d)](x,y-1)-gradx[abs(d)](x,y-3)+gradx[abs(d)](x,y+2);
			}
		}
	}
	disparity.Reset(left.Width(),left.Height());
	int dmin=0;
	for(y=0; y<left.Height(); y++)
	{
		for(x=0; x<left.Width(); x++)
		{
			temp(x,y)=10000;
			for(d=min; d<max; d++)
			{
				if(temp(x,y) > inter[abs(d)](x,y))
				{
					temp(x,y)=inter[abs(d)](x,y);
					dmin=abs(d);
				}
			}
			disparity(x,y)=dmin;
		}
	}
	return(disparity);
}


void PrintPly(ImgInt in, ImgBgr orig, int &depth) {

	
	
	FILE *file;
	fopen_s(&file,"plyfile","w");
	if(file == NULL) {
		BLEPO_ERROR("Error, cannot open file");
	} else {
		fprintf_s(file,"ply\nformat ascii 1.0\nelement vertex %d\nproperty float x\nproperty float y\nproperty float z\nproperty uchar diffuse_red\nproperty uchar diffuse_green\nproperty uchar diffuse_blue\nend_header\n",depth);
		ImgInt::Iterator p = in.Begin();
		ImgBgr::Iterator pOrig = orig.Begin();
		int halfHeight = in.Height() / 2, halfWidth = in.Width() / 2;
		float height = float(in.Height()), width = float(in.Width());
		for(int y = 0; y < in.Height(); y++) {
			for(int x = 0; x < in.Width(); x++) {
				float z = *p++ * DEPTH_CONST;
				if(z != 0)

					fprintf_s(file,"%lf %lf %lf %d %d %d\n",float(x-halfWidth) / width,float(halfHeight-y) / height,z,pOrig->r,pOrig->g,pOrig->b);
				pOrig++;
			}
		}
		fclose(file);
	}
}





