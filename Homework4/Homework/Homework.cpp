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

			if(argc == 3)
			{
			printf("Threshold value for cells_small.ppm is 35 and Threshold value for holes.ppm is 75 \n");

			CString path = "../../images/";
			CString filename = path + (CString) argv[1];
			int threshold = (int)atof(argv[2]);

			int sigma = 1;
			int mu = blepo_ex::Round(2.5f*sigma - 0.5f);
			int width = 2*mu + 1; //width = 5
			// since the width amounted to 5, then there will be 5 iteration of the gaussian
			float gaussian[5],Dgauss[5], gsum=0, Dsum=0;
			int label = 0;

			ImgBgr img;
			ImgGray Gimage1, Gimage2, Gimage3, Gimage4, Gimage5, Gimage6, Gimage7, Gimage8, Gimage9, Gimage10;
			ImgInt int1,int2,intimg,distance;
			ImgFloat TFimage, Fimage, Fimage2, Fimage3,Fimage4, Fimage5, Fimage6, Fimage7;
			int Hvalue;
			int Lvalue;
			int Htresh = 65;
		
			int high = 0;
			int low = 255;
			int x,y,i;

			int histogram[256];
			for(i=0; i<256; i++)
				histogram[i] = 0;
			
			Hvalue = threshold;
			Lvalue = Hvalue;

			Figure fig("Original Image");
				Figure fig1("Thresholded Image");
				Figure fig2("Chamfer Distance");
				Figure fig3("Watershed");
				 Figure fig4("Edge");
				  Figure fig5("Gradient Magnitude");
				  Figure fig6("Markers");
				  Figure fig7("Final Marker Watershed");
				 
				
			Load(filename, &img);
			Convert(img,&Gimage1);
			Convert(img,&Fimage);
			fig.Draw(img);

			unsigned char* pix = Gimage1.Begin();
			while(pix != Gimage1.End())
			{
				histogram[*pix]++;
				if(*pix<low)
					low = *pix;
				if(*pix<high)
					high = *pix;
				pix++;
			}

			Gimage2 = Gimage1;
			pix = Gimage2.Begin();
			while(pix != Gimage2.End())
			{
				if((*pix) < Htresh)
					*pix = 255;
				else
					*pix =0;
				pix++;
			}

			Gimage3 = Gimage1;
			pix = Gimage3.Begin();
			while(pix != Gimage3.End())
			{
				if((*pix) < Lvalue)
					*pix = 255;
				else
					*pix =0;
				pix++;
			}

			for(y=0;y<Gimage1.Height();y++)
			{
				for(x=0;x<Gimage1.Width();x++)
				{
					if((x+1)<Gimage1.Width() && Gimage3(x,y)==255)
						if(Gimage1(x+1,y)<Hvalue)
							Gimage1(x+1,y)=255;
					if((y+1)<Gimage1.Height() && Gimage3(x,y)==255)
						if(Gimage1(x,y+1)<Hvalue)
							Gimage3(x,y+1)=255;
				}
			}

			for(y=(Gimage1.Height()-1); y>0; y--)
			{
				for(x=(Gimage1.Width()-1); x>0; x--)
				{
					if((x-1)>0 && Gimage3(x,y)==255)
						if(Gimage1(x-1,y)<Hvalue)
							Gimage3(x-1,y)=255;
					if((y-1)>0 && Gimage3(x,y)==255)
						if(Gimage1(x,y-1)<Hvalue)
							Gimage3(x,y-1)=255;
				}
			}

			Gimage4 = Gimage3;

			for(y=1; y<(Gimage1.Height()-1); y++)
			{
				for(x=1; x<(Gimage1.Width()-1); x++)
				{
					if(Gimage3(x-1,y-1)==255 && Gimage3(x-1,y)==255 && Gimage3(x-1,y+1)==255 && Gimage3(x,y-1)==255 && Gimage3(x,y)==255 
						&& Gimage3(x,y+1)==255 && Gimage3(x+1,y-1)==255 && Gimage3(x+1,y)==255 && Gimage3(x+1,y+1)==255) 

						Gimage4(x,y) = 255;
					else
						Gimage4(x,y)=0;
				}
			}


			Gimage5 = Gimage4;

			for(y=1; y<(Gimage1.Height()-1); y++)
			{
				for(x=1; x<(Gimage1.Width()-1); x++)
				{
					if(Gimage4(x-1,y-1)==255 || Gimage4(x-1,y)==255 || Gimage4(x-1,y+1)==255 || Gimage4(x,y-1)==255 || Gimage4(x,y)==255 
						|| Gimage4(x,y+1)==255 || Gimage4(x+1,y-1)==255 || Gimage4(x+1,y)==255 || Gimage4(x+1,y+1)==255) 

						Gimage5(x,y) = 255;
					else
						Gimage5(x,y)=0;
				}
			}
			Gimage3 = Gimage5;

			fig1.Draw(Gimage3);

			Chamfer(Gimage3,distance);
			fig2.Draw(distance);

			std::deque<Point>prt;
			Point value;
			std::deque<Point>array[256];
			std::vector<ConnectedComponentProperties<ImgGray::Pixel> > reg;
			ConnectedComponents4(Gimage3, &int1, &reg);
			
			Convert(int1,&Gimage6);
			Gimage7 = Gimage6;

			for(y=0; y<int1.Height(); y++)
			{
				for(x=0; x<int1.Width(); x++)
				{
					Gimage7(x,y)=0;
					value.x=x;
					value.y=y;
					array[distance(x,y)].push_back(value);
				}
			}

			for(i=1; i<256; i++)
			{
				while(array[i].size() > 0)
				{
					value=array[i].front();
					array[i].pop_front();
					if(value.x < (Gimage6.Width()-1))
					{
						if((Gimage6(value.x+1, value.y) != 0) && (Gimage6(value.x, value.y)==0))
						{
							Gimage6(value.x, value.y)=Gimage6(value.x+1, value.y);
						}
						else if((Gimage6(value.x+1, value.y) != Gimage6(value.x, value.y))
							&& (Gimage6(value.x+1, value.y) != 0))
						{
							Gimage7(value.x, value.y)=255;
						}
					}

					if(value.y < (Gimage6.Height()-1))
					{
						if((Gimage6(value.x, value.y+1) != 0) 
							&& (Gimage6(value.x, value.y)==0))
						{
							Gimage6(value.x, value.y)= Gimage6(value.x, value.y+1);
						}
						else if((Gimage6(value.x, value.y+1) != Gimage6(value.x, value.y))
							&& (Gimage6(value.x, value.y+1) != 0))
						{
							Gimage7(value.x, value.y)=255;
						}
					}

					if(value.x > 0)
					{
						if((Gimage6(value.x-1, value.y) != 0) 
							&& (Gimage6(value.x, value.y)==0))
						{
							Gimage6(value.x, value.y)= Gimage6(value.x-1, value.y);
						}
						else if((Gimage6(value.x-1, value.y) != Gimage6(value.x, value.y))
							&& (Gimage6(value.x-1, value.y) != 0))
						{
							Gimage7(value.x, value.y)=255;
						}
					}

					if(value.y > 0)
					{
						if((Gimage6(value.x, value.y-1) != 0) 
							&& (Gimage6(value.x, value.y)==0))
						{
							Gimage6(value.x, value.y)= Gimage6(value.x, value.y-1);
						}
						else if((Gimage6(value.x, value.y-1) != Gimage6(value.x, value.y))
							&& (Gimage6(value.x, value.y-1) != 0))
						{
							Gimage7(value.x, value.y)=255;
						}
					}
				}
			}

						Convert(Gimage6,&intimg);
						fig3.Draw(intimg);
						fig4.Draw(Gimage7);

			
			for(i=0; i<5; i++)
			{
				gaussian[i]=expf((-1*((float)i-2)*((float)i-2))/ (2* sigma * sigma));
				Dgauss[i]=(((float)i-2)) * gaussian[i];
			}
			
			for(i=0; i<5; i++)
			{
				gsum+=gaussian[i];
				Dsum+=(1*Dgauss[i])*(float)(i-2);
			}
			
			for(i=0; i<5; i++)
			{
				gaussian[i]/=gsum;
				Dgauss[i]/=Dsum;
			}

			TFimage = Fimage;
			Fimage2 =  Fimage;
			Fimage3 =  Fimage;
			Fimage4 =  Fimage;

			for(y=2; y<(Gimage1.Height()-2); y++)
			{
				for(x=2; x<(Gimage1.Width()-2); x++)
				{
					TFimage(x,y)=(Fimage(x-2,y)*gaussian[0])+(Fimage(x-1,y)*gaussian[1])
						+(Fimage(x,y)*gaussian[2])+(Fimage(x+1,y)*gaussian[3])
						+(Fimage(x+2,y)*gaussian[4]);
				}
			}

			for(y=2; y<(Gimage1.Height()-2); y++)
			{
				for(x=2; x<(Gimage1.Width()-2); x++)
				{
					Fimage2(x,y)=(TFimage(x-2,y)*Dgauss[0])+(TFimage(x-1,y)*Dgauss[1])
						+(TFimage(x,y)*Dgauss[2])+(TFimage(x+1,y)*Dgauss[3])
						+(TFimage(x+2,y)*Dgauss[4]);
				}
			}

			for(y=2; y<(Gimage1.Height()-2); y++)
			{
				for(x=2; x<(Gimage1.Width()-2); x++)
				{
					TFimage(x,y)=(Fimage(x,y-2)*gaussian[0])+(Fimage(x,y-1)*gaussian[1])
						+(Fimage(x,y)*gaussian[2])+(Fimage(x,y+1)*gaussian[3])
						+(Fimage(x,y+2)*gaussian[4]);
				}
			}

			for(y=2; y<(Gimage1.Height()-2); y++)
			{
				for(x=2; x<(Gimage1.Width()-2); x++)
				{
					Fimage3(x,y)=(TFimage(x,y-2)*Dgauss[0])+(TFimage(x,y-1)*Dgauss[1])
						+(TFimage(x,y)*Dgauss[2])+(TFimage(x,y+1)*Dgauss[3])
						+(TFimage(x,y+2)*Dgauss[4]);
				}
			}

			for(y=0;y<Gimage1.Height();y++)
			{
				for(x=0;x<Gimage1.Width();x++)
				{
					Fimage4(x,y) = sqrt((Fimage2(x,y)*Fimage2(x,y)) + (Fimage3(x,y)*Fimage3(x,y)));
					if(x<2 || y<2 || y>=(Gimage1.Height()-2) || x>=(Gimage1.Width()-2))
						Fimage4(x,y)=0;
					
				}
			}

			fig5.Draw(Fimage4);
			
			Fimage5 = Fimage4;
			for(i=0; i<3; i++)
			{
				for(y=2; y<(Gimage1.Height()-2); y++)
				{
					for(x=2; x<(Gimage1.Width()-2); x++)
					{
						Fimage5(x,y)=(Fimage4(x-2,y)*gaussian[0])+(Fimage4(x-1,y)*gaussian[1])
							+(Fimage4(x,y)*gaussian[2])+(Fimage4(x+1,y)*gaussian[3])
							+(Fimage4(x+2,i)*gaussian[4]);
					}
				}
			

				Fimage6 = Fimage5;
				for(y=2; y<(Gimage1.Height()-2); y++)
				{
					for(x=2; x<(Gimage1.Width()-2); x++)
					{
						Fimage6(x,y)=(Fimage5(x-2,y)*gaussian[0])+(Fimage5(x-1,y)*gaussian[1])
							+(Fimage5(x,y)*gaussian[2])+(Fimage5(x+1,y)*gaussian[3])
							+(Fimage5(x+2,i)*gaussian[4]);
					}
				}
				Fimage4 = Fimage6;
			}

				Gimage8 = Gimage7;
			for(y=0; y<Gimage1.Height(); y++)
			{
				for(x=0; x<Gimage1.Width(); x++)
				{
					if(Gimage3(x,y)==255)
						Gimage8(x,y)=255;
				}
			}
			fig6.Draw(Gimage8);

			
			Gimage9 = Gimage8;
			std::deque<Point> point;
			
			for(y=0; y<int1.Height(); y++)
			{
				for(x=0; x<int1.Width(); x++)
				{
					Gimage9(x,y)=0;
				}
			}

			for(y=0; y<int1.Height(); y++)
			{
				for(x=0; x<int1.Width(); x++)
				{
					if(Gimage8(x,y)==255 && Gimage9(x,y)==0)
					{
						value.x=x;
						value.y=y;
						point.push_back(value);
						label++;	
						while (point.size() > 0)
						{
							value = point.back();
							point.pop_back();
							if(Gimage8(value.x,value.y) == 255)
								Gimage9(value.x, value.y) = label;
						if((value.x < (Gimage8.Width()-1))
								&& (Gimage8(value.x+1, value.y) == 255))
							{
								if(Gimage9(value.x+1,value.y)==0)
									point.push_back(Point(value.x+1,value.y));
							}
						if((value.y < (Gimage8.Height()-1))
								&& (Gimage8(value.x, value.y+1) == 255))
							{
								if(Gimage9(value.x,value.y+1)==0)
									point.push_back(Point(value.x,value.y+1));
							}
						if((value.x > 0)
								&& (Gimage8(value.x-1, value.y) == 255))
							{
								if(Gimage9(value.x-1,value.y)==0)
								point.push_back(Point(value.x-1,value.y));
							}
						if((value.y > 0)
								&& (Gimage8(value.x, value.y-1) == 255))
							{
								if(Gimage9(value.x,value.y-1)==0)
								point.push_back(Point(value.x,value.y-1));
							}
						if((value.x > 0 && value.y >0)
								&& (Gimage8(value.x-1, value.y-1) == 255))
							{
								if(Gimage9(value.x-1,value.y-1)==0)
								point.push_back(Point(value.x-1,value.y-1));
							}
						if((value.x > 0 && value.y < Gimage8.Height()-1)
								&& (Gimage8(value.x-1, value.y+1) == 255))
							{
								if(Gimage9(value.x-1,value.y+1)==0)
								point.push_back(Point(value.x-1,value.y+1));
							}
						if((value.x < (Gimage8.Width()-1) && value.y >0)
								&& (Gimage8(value.x+1, value.y-1) == 255))
							{
								if(Gimage9(value.x+1,value.y-1)==0)
								point.push_back(Point(value.x+1,value.y-1));
							}
						if((value.x < (Gimage8.Width()-1) && value.y < Gimage8.Height()-1)
								&& (Gimage8(value.x+1, value.y+1) == 255))
							{
								if(Gimage9(value.x+1,value.y+1)==0)
								point.push_back(Point(value.x+1,value.y+1));
							}

						}

					}
				}
			}
						Convert(Fimage6,&Gimage10);
						for(y=0; y<int1.Height(); y++)
						{
						for(x=0; x<int1.Width(); x++)
						{
								value.x=x;
								value.y=y;
								array[Gimage10(x,y)].push_back(value);
						}
					}
							Bgr color = Bgr(0,255,0);
							Point value2;

				for(i=1; i<256; i++)
				{
				
				while(array[i].size() > 0)
				{
					value=array[i].front();
					array[i].pop_front();
					if(value.x < (Gimage6.Width()-1))
					{
						if((Gimage9(value.x+1, value.y) != 0) 
							&& (Gimage9(value.x, value.y)==0))
						{
							Gimage9(value.x, value.y)= Gimage9(value.x+1, value.y);
							value2.x=value.x;
							value2.y=value.y;
							point.push_back(value2);
						}
					}

					if(value.x < (Gimage6.Width()-1) && value.y <(Gimage6.Height()-1))
					{
						if((Gimage9(value.x+1, value.y+1) != 0) 
							&& (Gimage9(value.x, value.y)==0))
						{
							Gimage9(value.x, value.y)=Gimage9(value.x+1, value.y+1);
							value2.x=value.x;
							value2.y=value.y;
							point.push_back(value2);
						}
					}
					if(value.x < (Gimage6.Width()-1) && value.y > 0)
					{
						if((Gimage9(value.x+1, value.y-1) != 0) 
							&& (Gimage9(value.x, value.y)==0))
						{
							Gimage9(value.x, value.y)=Gimage9(value.x+1, value.y-1);
							value2.x=value.x;
							value2.y=value.y;
							point.push_back(value2);
						}
					}
					if(value.y < (Gimage6.Height()-1))
					{
						if((Gimage9(value.x, value.y+1) != 0) 
							&& (Gimage9(value.x, value.y)==0))
						{
							Gimage9(value.x, value.y)=Gimage9(value.x, value.y+1);
							value2.x=value.x;
							value2.y=value.y;
							point.push_back(value2);
						}
					}
					if(value.x > 0)
					{
						if((Gimage9(value.x-1, value.y) != 0) 
							&& (Gimage9(value.x, value.y)==0))
						{
							Gimage9(value.x, value.y)=Gimage9(value.x-1, value.y);
							value2.x=value.x;
							value2.y=value.y;
							point.push_back(value2);
						}
					}
					if(value.x > 0 && value.y > 0)
					{
						if((Gimage9(value.x-1, value.y-1) != 0) 
							&& (Gimage9(value.x, value.y)==0))
						{
							Gimage9(value.x, value.y)=Gimage9(value.x-1, value.y-1);
							value2.x=value.x;
							value2.y=value.y;
							point.push_back(value2);
						}
					}
					if(value.x > 0 && value.y < (Gimage6.Height()-1))
					{
						if((Gimage9(value.x-1, value.y+1) != 0) 
							&& (Gimage9(value.x, value.y)==0))
						{
							Gimage9(value.x, value.y)=Gimage9(value.x-1, value.y+1);
							value2.x=value.x;
							value2.y=value.y;
							point.push_back(value2);
						}
					}
					if(value.y > 0)
					{
						if((Gimage9(value.x, value.y-1) != 0) 
							&& (Gimage9(value.x, value.y)==0))
						{
							Gimage9(value.x, value.y)=Gimage9(value.x, value.y-1);
							value2.x=value.x;
							value2.y=value.y;
							point.push_back(value2);
						}
					}
				}
						
					while(point.size() > 0)
					{
					value=point.front();
					point.pop_front();
					if(value.x < (Gimage6.Width()-1))
					{
						if((Gimage10(value.x+1, value.y) <= i)
							&& (Gimage9(value.x+1, value.y)==0))
						{
							Gimage9(value.x+1, value.y)= Gimage9(value.x, value.y);
							value2.x=value.x+1;
							value2.y=value.y;
							point.push_back(value2);
						}
					}
					if(value.x < (Gimage6.Width()-1) && value.y>0)
					{
						if((Gimage10(value.x+1, value.y-1) <= i)
							&& (Gimage9(value.x+1, value.y-1)==0))
						{
							Gimage9(value.x+1, value.y-1)= Gimage9(value.x, value.y);
							value2.x=value.x+1;
							value2.y=value.y-1;
							point.push_back(value2);
						}
					}
					if(value.x < (Gimage6.Width()-1) && value.y < (Gimage6.Height()-1))
					{
						if((Gimage10(value.x+1, value.y+1) <= i)
							&& (Gimage9(value.x+1, value.y+1)==0))
						{
							Gimage9(value.x+1, value.y+1)= Gimage9(value.x, value.y);
							value2.x=value.x+1;
							value2.y=value.y+1;
							point.push_back(value2);
						}
					}
					if(value.y < (Gimage6.Height()-1))
					{
						if((Gimage10(value.x, value.y+1) <= i)
							&& (Gimage9(value.x, value.y+1)==0))
						{
							Gimage9(value.x, value.y+1)= Gimage9(value.x, value.y);
							value2.x=value.x;
							value2.y=value.y+1;
							point.push_back(value2);
						}
					}
					if(value.x > 0)
					{
						if((Gimage10(value.x-1, value.y) <= i)
							&& (Gimage9(value.x-1, value.y)==0))
						{
							Gimage9(value.x-1, value.y)= Gimage9(value.x, value.y);
							value2.x=value.x-1;
							value2.y=value.y;
							point.push_back(value2);
						}
					}
					if(value.x > 0 && value.y > 0)
					{
						if((Gimage10(value.x-1, value.y-1) <= i)
							&& (Gimage9(value.x-1, value.y-1)==0))
						{
							Gimage9(value.x-1, value.y-1)= Gimage9(value.x, value.y);
							value2.x=value.x-1;
							value2.y=value.y-1;
							point.push_back(value2);
						}
					}
					if(value.x > 0 && value.y < (Gimage6.Height()-1))
					{
						if((Gimage10(value.x-1, value.y+1) <= i)
							&& (Gimage9(value.x-1, value.y+1)==0))
						{
							Gimage9(value.x-1, value.y+1)= Gimage9(value.x, value.y);
							value2.x=value.x-1;
							value2.y=value.y+1;
							point.push_back(value2);
						}
					}
					if(value.y > 0)
					{
						if((Gimage10(value.x, value.y-1) <= i)
							&& (Gimage9(value.x, value.y-1)==0))
						{
							Gimage9(value.x, value.y-1)= Gimage9(value.x, value.y);
							value2.x=value.x;
							value2.y=value.y-1;
							point.push_back(value2);
						}
					}

				}
			}

					for(y=1; y<int1.Height()-1; y++)
					{
						for(x=1; x<int1.Width()-1; x++)
						{
							if(Gimage9(x,y+1)!=Gimage9(x,y) || Gimage9(x+1,y)!=Gimage9(x,y)
							|| Gimage9(x-1,y)!=Gimage9(x,y) || Gimage9(x,y-1)!=Gimage9(x,y))
							img(x,y)=color;
						}
					}
			      fig7.Draw(img);

	
			} 
			else 
			{
				printf("invalid amount of command-line parameters\n Threshold value for cells_small.ppm is 35 and Threshold value for holes.ppm is 75 \n");
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

void Chamfer(ImgGray &Gimage1, ImgInt &distance) {
	distance.Reset(Gimage1.Width(), Gimage1.Height());
	const int max = Gimage1.Width() * Gimage1.Height() + 1;
	int i, j;
	const int width = Gimage1.Width() - 1, height = Gimage1.Height() - 1;

	ImgGray::ConstIterator pImg = Gimage1.Begin();
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
	pImg = Gimage1.Begin(width,height);
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
 




