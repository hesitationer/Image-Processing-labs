//Name: Christopher Cornwall

// Homework.cpp : Defines the entry point for the console application.
//

//

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

double min;

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

			if(argc == 6)
			{
printf("When checking Statue_seq: First_frame = 588 and last_frame = 618\nAnd change make the change that is commented in line 63\nFor Flowergarden: first_frame = 31 and last_frame = 58\n");
			
			
        CString filename;
		CString path = "../../images/";
		CString  format = path + (CString) argv[1];


		// When checking statue_seq CHANGE "flowergarden/img%03d.pgm" to "statue_seq/img%04d.bmp"
		CString format2 = path + "flowergarden/img%03d.pgm"; 


		int first_frame = atoi(argv[2]);
		int last_frame = atoi(argv[3]);
		float sigma = atof(argv[4]);
		int window_size = atoi(argv[5]);

		filename.Format(format);
		
		
		CString trackfile;
		ImgBgr img, img2, img3;
		ImgGray Gimage,Gimage2,gimg3,gimg4;
		Figure fig("Feature Overlaid");
	    Figure fig2("Track Features");
		int count,i,j,k;
		Load(filename, &img);
		Convert(img,&Gimage);
		DetectingFeatures(Gimage);


		// Computing the features Overlaid on image for the First Window
		Convert(Gimage,&img3);
		for(count=0; count<100; count++)
		{
			for(j=-1; j<=1; j++)
			{
				for(i=-1; i<=1; i++)
				{
					img3(feature[count].x+j,feature[count].y+i)=Bgr(0,0,255);
				}
			}
		}
		
		fig.Draw(img3);

		trackfile = filename;
		
		// Tracking the Features as time progress in next window
		for (k = first_frame; k <= last_frame; k++)
		{
				
			trackfile.Format(format2, k);

			Load(trackfile, &img2);
			Convert(img2,&Gimage2);
			LKTrackingFeatures(Gimage, Gimage2, window_size);
			img3=img2;
			Gimage=Gimage2;

			
			for(count=0; count<100; count++)
			{
				if(feature[count].val==0)
					continue;
				for(j=-1; j<=1; j++)
				{
					for(i=-1; i<=1; i++)
					{
						if(((feature[count].x+j) > 0) && ((feature[count].x+j) < (Gimage2.Width()-1))
							&& ((feature[count].y+i) > 0) && ((feature[count].y+i) < (Gimage2.Height()-1)))
							img3(feature[count].x+j,feature[count].y+i)=Bgr(0,0,255);
					}
				}
			}
			fig2.Draw(img3);

		}	
		
	
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


void DetectingFeatures(ImgGray Gimage)
{
	ImgFloat temp,orig,smooth;
	ImgFloat gradx,grady;
	int i,j,k,l;

	Convert(Gimage,&gradx);
	grady=gradx;
	orig=gradx;
	temp=gradx;
	smooth=gradx;

	
	SmoothAndGradient(orig,sigma,&smooth,&gradx,&grady,&temp);

	int featcount=0,count,count2,flag=0,check=0;
	for(count=0; count<100; count++)
	{
		feature[count].x=0;
		feature[count].y=0;
		feature[count].val=0;
	}
	
	for(i=4; i<(Gimage.Height()-4); i++)
	{
		for(j=4; j<(Gimage.Width()-4); j++)
		{
			gxx=0;
			gyy=0;
			gxy=0;
			for(k=-4; k<=4; k++)
			{
				for(l=-4; l<=4; l++)
				{
					gxx+=(gradx(k+j,l+i)*gradx(k+j,l+i));
					gyy+=(grady(k+j,l+i)*grady(k+j,l+i));
					gxy+=(gradx(k+j,l+i)*grady(k+j,l+i));
				}
			}
			
			double min = (gxx + gyy - sqrt(4*gxy*gxy + (gxx - gyy)*(gxx - gyy)))/2;
			
			if(featcount<100 && min!=0)
			{
				
				check=0;
				for(count=0; count<featcount; count++)
				{
					if(feature[count].val != 0)
					{
						if(sqrt(SQR(feature[count].x-j) + SQR(feature[count].y-i)) <= dist)
							check=1;
					}
				}
				if(check==0)
				{
					feature[featcount].val=min;
					feature[featcount].x=j;
					feature[featcount].y=i;
					featcount++;
				}
			}
			else if(featcount==100 && flag==0)
			{
				
				for(count=99; count>=0; count--)
				{
					for(count2=0; count2<count; count2++)
					{
						if(feature[count].val < feature[count2].val)
						{
							feattemp=feature[count];
							feature[count]=feature[count2];
							feature[count2]=feattemp;
						}
					}
				}
				flag=1;
				
				check=0;
				for(count=0; count<100; count++)
				{
					if(feature[count].val != 0)
					{
						if(sqrt(SQR(feature[count].x-j) + SQR(feature[count].y-i)) <= dist)
							check=1;
					}
				}
				if(check==0)
				{
					for(count=0; count<100; count++)
					{
						if(feature[count].val > min)
						{
							for(count2=1; count2<=count; count2++)
							{
								feattemp=feature[count2];
								feature[count2]=feature[count2-1];
								feature[count2-1]=feattemp;
							}
							feature[count].val=min;
							feature[count].x=j;
							feature[count].y=i;
							featcount++;
							break;
						}
					}
				}
			}
			else 
			{
				
				check=0;
				for(count=0; count<100; count++)
				{
					if(feature[count].val != 0)
					{
						if(sqrt(SQR(feature[count].x-j) + SQR(feature[count].y-i)) <= dist)
							check=1;
					}
				}
				if(check==0)
				{
					for(count=0; count<100; count++)
					{
						if(feature[count].val > min)
						{
							for(count2=1; count2<=count; count2++)
							{
								feattemp=feature[count2];
								feature[count2]=feature[count2-1];
								feature[count2-1]=feattemp;
							}
							feature[count].val=min;
							feature[count].x=j;
							feature[count].y=i;
							featcount++;
							break;
						}
					}
				}
			}
		}
	}
	return;
}



float Interpolate(ImgFloat img1,float x1, float y1)
{
	int x0,y0;
	float ax,ay;
	x0=(int)x1;
	y0=(int)y1;
	ax=x1-x0;
	ay=y1-y0;
	return(   ((1-ax) * (1-ay) * img1(x0,   y0))
			+ (ax      * (1-ay) * img1(x0+1, y0))
			+ ((1-ax)  *  ay    * img1(x0,   y0+1))
			+ (ax      *  ay    * img1(x0+1, y0+1))
			);
}


void LKTrackingFeatures(ImgGray Gimage, ImgGray Gimage2, int window_size)
{
	ImgFloat temp,orig,smooth;
	ImgFloat gradx,grady;
	int i,j,k,l;
	

	Convert(Gimage,&gradx);
	grady=gradx;
	orig=gradx;
	temp=gradx;
	smooth=gradx;

	
	SmoothAndGradient(orig,sigma,&smooth,&gradx,&grady,&temp);

	double u=0,v=0;
	ImgFloat Fimage,Fimage2;
	Convert(Gimage,&Fimage);
	Convert(Gimage2,&Fimage2);
	ImgFloat Gimagew,gradxw,gradyw,Gimage2w;
	Gimagew.Reset(window_size,window_size);
	Gimage2w.Reset(window_size,window_size);
	gradxw.Reset(window_size,window_size);
	gradyw.Reset(window_size,window_size);
	for(k=0; k<100; k++)
	{
		if(feature[k].val==0)
			continue;
		
		for(j=-4; j<=4; j++)
		{
			for(i=-4; i<=4; i++)
			{
				if(((feature[k].x+j) > 0) && ((feature[k].y+i) > 0)
					&& ((feature[k].x+j) < (Fimage.Width()-1)) && ((feature[k].y+i) < (Fimage.Height()-1)))
				{
					Gimagew(j+4,i+4)=Interpolate(Fimage,feature[k].x+j,feature[k].y+i);
					gradxw(j+4,i+4)=Interpolate(gradx,feature[k].x+j,feature[k].y+i);
					gradyw(j+4,i+4)=Interpolate(grady,feature[k].x+j,feature[k].y+i);
				}
				else 
				{
					Gimagew(j+4,i+4)=0;
					gradxw(j+4,i+4)=0;
					gradyw(j+4,i+4)=0;
				}
			}
		}
		gxx=0;
		gyy=0;
		gxy=0;


		
		for(i=0; i<window_size; i++)
		{
			for(l=0; l<window_size; l++)
			{
				gxx+=(gradxw(i,l)*gradxw(i,l));
				gyy+=(gradyw(i,l)*gradyw(i,l));
				gxy+=(gradxw(i,l)*gradyw(i,l));
			}
		}
		

		u=0;
		v=0;
		flag=0;

		do
		{
			
			for(j=-4; j<=4; j++)
			{
				for(i=-4; i<=4; i++)
				{
					if(((feature[k].x+u+j) > 0) && ((feature[k].y+v+i) > 0)
						&& ((feature[k].x+u+j) < (Fimage.Width()-1)) && ((feature[k].y+v+i) < (Fimage.Height()-1)))
						Gimage2w(j+4,i+4)=Interpolate(Fimage2,feature[k].x+u+j,feature[k].y+v+i);
					else Gimage2w(j+4,i+4)=0;
				}
			}
			ex=0;
			ey=0;

			
			for(j=0; j<window_size; j++)
			{
				for(i=0; i<window_size; i++)
				{
					ex+=gradxw(j,i)*(Gimagew(j,i)-Gimage2w(j,i));
					ey+=gradyw(j,i)*(Gimagew(j,i)-Gimage2w(j,i));
				}
			}
			dx=0;
			dy=0;
			
			det=(gxx*gyy) - (gxy*gxy);
			if(det==0)
			{
				feature[k].val=0;
				break;
			}
			dx=((gyy*ex) - (gxy*ey))/det;
			dy=((gxx*ey) - (gxy*ex))/det;
			
			u+=dx;
			v+=dy;
			flag++;
		}while((sqrt((dx*dx) + (dy*dy)) >= epsilon) && (flag <= 20));
		if(flag>10 || feature[k].val==0)
		{
			feature[k].x=0;
			feature[k].y=0;
			feature[k].val=0;
		}
		else
		{
			feature[k].x=feature[k].x+u;
			feature[k].y=feature[k].y+v;
		}
	}
	return;
}



