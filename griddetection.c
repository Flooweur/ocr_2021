#include "SDL/SDL.h"
#include <stdio.h>
#include <err.h>
#include "SDL/SDL_rotozoom.h"
#include "SDL/SDL_image.h"
#include "griddetection.h"
#include <math.h>
#include "pixel_operations.h"
#include <limits.h>

void HoughTransformation(char* image,char* outname)
{
	SDL_Surface* surface = SDL_LoadBMP(image);
	if(!surface)
		errx(3,"can't load %s,%s",image,SDL_GetError());
	int h = surface->h;
	int w = surface->w;
	double rho = 1.0;
	double theta =1.0;
	int Ntheta = (int) (180.0/theta);
	int Nrho = (int) (floor(sqrt(h*h+w*w))/rho);
	double dtheta = M_PI/Ntheta;
	double drho = floor(sqrt(h*h+w*w)) /Nrho;
	unsigned int* accu = (unsigned int*) calloc(Ntheta*Nrho,sizeof(unsigned int));
	
	Uint8 red, green,blue;
	Uint32 pixel;
	//fill accumulator
	for(int y = 0; y < h; y++)
		for(int x = 0; x<w; x++)
		{
			pixel = get_pixel(surface,x,y);
			SDL_GetRGB(pixel,surface->format,&red,&green,&blue);
			if(pixel != 0)
			{
				for(int t = 0; t < Ntheta; t++)
				{
					theta = t * dtheta;
					rho = (double) x *cos(theta) +(double) y*sin(theta);
					int r = (int) (rho/drho);
					if(r>0 && r < Nrho)
						accu[r*Ntheta+t] ++;
				}
			}
		}
	//select maxima and draw lines and intersections
	int threshold = 160;
	for(int it =0; it < Ntheta;it++)
		for(int ir=0; ir < Nrho; ir++)
		{
			if((int)accu[ir*Ntheta+it] >= threshold)
			{
				int max = accu[ir*Ntheta+it];
				for(int ly = -8;ly <= 8;ly++)
				{
					for(int lx = -8; lx <= 8; lx++)
					{
						if((ly+ir >= 0 && ly + ir < Nrho) &&(lx+it>=0 && lx+it<Ntheta))
						{
							if((int)accu[(ir+ly)*Ntheta + (it+lx)] > max)
							{
								max = accu[(ir+ly)*Ntheta+(it+lx)];
								lx=ly=9;
							}
						}
					}
				}
				if(max > (int)accu[ir*Ntheta+it])
					continue;
				rho = ir*drho;
				theta = it*dtheta;
				double a = cos(theta);
				double b = sin(theta);
				if(it == 90)
				{
					for(int x = 0; x < w; x++)
					{
						int y = (int) (rho-x*a)/b;
						if(y >= 0 && y < h)
						{
							pixel = get_pixel(surface,x,y);
							SDL_GetRGB(pixel,surface->format,&red,&green,&blue);
							if(green == 255 && blue ==0 && red ==0)
								put_pixel(surface,x,y, SDL_MapRGB(surface->format,255,0,0));
							else
								put_pixel(surface,x,y,SDL_MapRGB(surface->format,0,0,255));
						}
					}
				}
				else if(it == 0) 
				{
					for(int y = 0; y < h; y++)
					{
						int x= (int) (rho-y*b)/a;
						if(x >= 0 && x < w)
						{
							pixel = get_pixel(surface,x,y);
							SDL_GetRGB(pixel,surface->format,&red,&green,&blue);
							if(blue == 255 && green ==0 && red == 0)
								put_pixel(surface,x,y,SDL_MapRGB(surface->format,255,0,0));
							else
								put_pixel(surface,x,y,SDL_MapRGB(surface->format,0,255,0));								
						}
					}
				}
			}
		}
	SDL_SaveBMP(surface, outname);
	SDL_FreeSurface(surface);	
	free(accu);
}
int findAngle(unsigned int* accu, int Ntheta, int Nrho)
{
	int max = 0;
	int temp = 0;
	int angle  =0;
	int count = 0;
	for(int t = 90; t <= 135; t++)
	{
		for(int r =0; r < Nrho; r++)
		{
			if((int)accu[r*Ntheta+t]> 100)
			{
				count += 1;
		       		temp += accu[r*Ntheta+t];
			}
		}
		temp = temp/count;
		if(max < temp)
		{
			max = temp;
			angle = t;
		}
		temp = 0;
		count = 0;
	}
	return angle;
}
int HoughTransformation2(char* image,char* outname, int threshold)
{
	SDL_Surface* surface = SDL_LoadBMP(image);
	if(!surface)
		errx(3,"can't load %s,%s",image,SDL_GetError());
	int h = surface->h;
	int w = surface->w;
	double rho = 1.0;
	double theta =1.0;
	int Ntheta = (int) (180.0/theta);
	int Nrho = (int) (floor(sqrt(h*h+w*w))/rho);
	double dtheta = M_PI/Ntheta;
	double drho = floor(sqrt(h*h+w*w)) /Nrho;
	unsigned int* accu = (unsigned int*) calloc(Ntheta*Nrho,sizeof(unsigned int));
	
	Uint8 red, green,blue;
	Uint32 pixel;
	//fill accumulator
	for(int y = 0; y < h; y++)
		for(int x = 0; x<w; x++)
		{
			pixel = get_pixel(surface,x,y);
			SDL_GetRGB(pixel,surface->format,&red,&green,&blue);
			if(pixel != 0)
			{
				for(int t = 0; t < Ntheta; t++)
				{
					theta = t * dtheta;
					rho = (double) x *cos(theta) +(double) y*sin(theta);
					int r = (int) (rho/drho);
					if(r>0 && r < Nrho)
						accu[r*Ntheta+t] ++;
				}
			}
		}
	//select maxima and draw lines and intersections
	int angle = findAngle(accu,Ntheta,Nrho);

	for(int it =0; it < Ntheta;it++)
		for(int ir=0; ir < Nrho; ir++)
		{
			if((int)accu[ir*Ntheta+it] >= threshold)
			{
				int max = accu[ir*Ntheta+it];
				//local maxima
				for(int ly = -8;ly <= 8;ly++)
				{
					for(int lx = -8; lx <= 8; lx++)
					{
						if((ly+ir >= 0 && ly + ir < Nrho) &&(lx+it>=0 && lx+it<Ntheta))
						{
							if((int)accu[(ir+ly)*Ntheta + (it+lx)] > max)
							{
								max = accu[(ir+ly)*Ntheta+(it+lx)];
								lx=ly=9;
							}
						}
					}
				}
				
				if(max > (int)accu[ir*Ntheta+it])
					continue;
				rho = ir*drho;
				theta = it*dtheta;
				double a = cos(theta);
				double b = sin(theta);
				if(it >= 45 && it <= 135)
				{
					for(int x = 0; x < w; x++)
					{
						int y = (int) (rho-x*a)/b;
						if(y >= 0 && y < h)
						{
							pixel = get_pixel(surface,x,y);
							SDL_GetRGB(pixel,surface->format,&red,&green,&blue);
							if(green == 255 && blue ==0 && red ==0)
								put_pixel(surface,x,y, SDL_MapRGB(surface->format,255,0,0));
							else
								put_pixel(surface,x,y,SDL_MapRGB(surface->format,0,0,255));
						}
					}
				}
				else
				{
					for(int y = 0; y < h; y++)
					{
						int x= (int) (rho-y*b)/a;
						if(x >= 0 && x < w)
						{
							pixel = get_pixel(surface,x,y);
							SDL_GetRGB(pixel,surface->format,&red,&green,&blue);
							if(blue == 255 && green ==0 && red == 0)
								put_pixel(surface,x,y,SDL_MapRGB(surface->format,255,0,0));
							else
								put_pixel(surface,x,y,SDL_MapRGB(surface->format,0,255,0));								
						}
					}
				}
			}
		}
	SDL_SaveBMP(surface, outname);
	SDL_FreeSurface(surface);	
	free(accu);
	return angle;
}
struct Coordonnates{
	int x;
	int y;
};

void rotateauto(char* image, double angle,char* result)
{
	SDL_Surface* surface = SDL_LoadBMP(image);
	if(!surface)
		errx(3,"can't load %s,%s",image,SDL_GetError());
	SDL_SaveBMP(rotozoomSurface(surface,angle,1,0),"rotated.bmp");

	SDL_Surface* surfaceRot = SDL_LoadBMP("rotated.bmp");
	double deg = M_PI/180.0;
	double sinan = sin(angle*deg);
	if(sinan < 0)
		sinan = sinan * (-1);
	int x1 = (sinan * surface->h)/2;
	int x2 = surfaceRot->w -x1;
	int y1 = (sinan * surface->w)/2;
	int y2 = surfaceRot->h - y1;
	SDL_Surface* surfaceCropped = SDL_CreateRGBSurface(0,x2-x1,y2-y1,32,0,0,0,0);
	for(int y = 0; y < surfaceCropped->h; y++)
	{
		for(int x = 0; x < surfaceCropped->w;x++)
		{
			put_pixel(surfaceCropped,x,y,(get_pixel(surfaceRot,x1+x,y1+y)));
		}
	}
	SDL_SaveBMP(surfaceCropped,result);
	SDL_FreeSurface(surfaceCropped);
	SDL_FreeSurface(surfaceRot);
	SDL_FreeSurface(surface);
}
void SobelFilter(char *image,char* name)
{
	SDL_Surface* surface = SDL_LoadBMP(image);
	if(!surface)
		errx(3,"can't load %s,%s",image,SDL_GetError());
	int h = surface->h;
	int w = surface->w;
	SDL_Surface* result = SDL_CreateRGBSurface(0,w,h,32,0,0,0,0);	
	int gx =0;
	int gy =0;
	for(int y =2; y<h-2; y++)
		for(int x =2; x < w-2; x++)
		{
			gx = 0;
			gy =0;
			Uint8 red,green,blue;
			//topleft 
			Uint32 pixel = get_pixel(surface,x-1,y-1);
			SDL_GetRGB(pixel,surface->format,&red,&green,&blue);
			gx += -(red+green+blue);
			gy += -(red+green+blue);

			//middle left
			pixel = get_pixel(surface,x-1,y);
			SDL_GetRGB(pixel,surface->format,&red,&green,&blue);
			gx += -2 * (red+green+blue);

			//downleft
			pixel = get_pixel(surface,x-1,y+1);
			SDL_GetRGB(pixel,surface->format,&red,&green,&blue);
			gx += -(red+green+blue);
			gy+= (red+green+blue);

			//top middle
			pixel =get_pixel(surface,x,y-1);
			SDL_GetRGB(pixel,surface->format,&red,&green,&blue);
			gy += -2 *(red+green+blue);

			//down middle
			pixel = get_pixel(surface,x,y+1);
			SDL_GetRGB(pixel,surface->format,&red,&green,&blue);
			gy += 2*(red+green+blue);

			//top right
			pixel = get_pixel(surface,x+1,y-1);
			SDL_GetRGB(pixel,surface->format,&red,&green,&blue);
			gx+= (red+green+blue);
			gy+= -(red+green+blue);

			//middle right
			pixel = get_pixel(surface,x+1,y);
			SDL_GetRGB(pixel,surface->format,&red,&green,&blue);
			gx+= 2*(red+green+blue);

			//down right
			pixel = get_pixel(surface,x+1,y+1);
			SDL_GetRGB(pixel,surface->format,&red,&green,&blue);
			gx+= (red+green+blue);
			gy += (red+green+blue);

			double g = sqrt((double) (gx*gx+gy*gy));
			g = g/(255*3) * 255;

			Uint8 newcolor = (Uint8)g;
			put_pixel(result,x,y,SDL_MapRGB(surface->format,newcolor,newcolor,newcolor));

		}
	SDL_SaveBMP(result, name);
	SDL_FreeSurface(surface);
	SDL_FreeSurface(result);	
}

void blackandwhiteinversion(char* image)
{
	SDL_Surface* surface = SDL_LoadBMP(image);
	if(!surface)
		errx(3,"can't load %s,%s",image,SDL_GetError());
	for(int y = 0; y <surface->h;y++)
		for(int x = 0; x< surface->w;x++)
		{
			Uint32 pixel = get_pixel(surface,x,y);
			if(pixel == 0)
				put_pixel(surface,x,y,SDL_MapRGB(surface->format,255,255,255));
			else
				put_pixel(surface,x,y,SDL_MapRGB(surface->format,0,0,0));

		}
	SDL_SaveBMP(surface,"inversion.bmp");
	SDL_FreeSurface(surface);
}
void Dilatation(char* image,char* name)
{
	SDL_Surface* surface = SDL_LoadBMP(image);
	if(!surface)
		errx(3,"can't load %s,%s",image,SDL_GetError());
	int h = surface->h;
	int w = surface->w;
	SDL_Surface* result = SDL_CreateRGBSurface(0,w,h,32,0,0,0,0);
	for(int y = 0; y<h;y++)
		for(int x =0; x<w;x++)
		{
			Uint32 pixel = get_pixel(surface,x,y);
			if(pixel != 0)
			{
				if(x-1 >= 0)
					put_pixel(result,x-1,y,SDL_MapRGB(result->format,255,255,255));
				if(y-1 >= 0)
					put_pixel(result,x,y-1,SDL_MapRGB(result->format,255,255,255));
				if(y+1 < h)
					put_pixel(result,x,y+1,SDL_MapRGB(result->format,255,255,255));
				if(x+1 <w)
					put_pixel(result,x+1,y,SDL_MapRGB(result->format,255,255,255));

			}
		}
	SDL_SaveBMP(result,name);
	SDL_FreeSurface(surface);
	SDL_FreeSurface(result);
}

Uint32 Find(Uint32 pixel,Uint32* labels)
{
	Uint32 pixel2 = pixel;
	while(labels[pixel2] != pixel2)
		pixel2=labels[pixel2];
	while(labels[pixel] != pixel)
	{
		Uint32 pixel3 = labels[pixel];
		labels[pixel]= pixel2;
		pixel = pixel3;
	}
	return pixel2;
}

void unionBLOB(Uint32 x,Uint32 y,Uint32* labels)
{
	labels[Find(x,labels)] = Find(y,labels);
}

int BLOBdetection(char* image)
{
	SDL_Surface* surface = SDL_LoadBMP(image);
	if(!surface)
		errx(3,"can't load %s,%s",image,SDL_GetError());
	Uint32 largest_label = 0;
	int largest_label_int =0;
	SDL_Surface* result = SDL_CreateRGBSurface(0,surface->w,surface->h,32,0,0,0,0);
	Uint32* labels = (Uint32*)calloc(surface->w*surface->h,sizeof(Uint32));
	
	Uint32 index = 0;
	Uint32* label = result->pixels;
	int h = result->h;
	int w = result->w;
	
	for(int y =0; y <h;y++)
		for(int x = 0; x<w;x++)
		{
			labels[y*w+x] = index;
			index++;
		}
	Uint32 pixel;
	Uint8 r,g,b;
	for(int y =0; y <h; y++)
		for(int x = 0; x<w;x++)
		{
			pixel = get_pixel(surface,x,y);
			SDL_GetRGB(pixel,surface->format,&r,&g,&b);
			if(r!=0)
			{	
				Uint32 left = 0;
				Uint32 above =0;
				if(x-1 > 0)
					left = get_pixel(result,x-1,y);
				if(y-1>0)
					above = get_pixel(result,x,y-1);
				if((left == 0) && (above ==0))
				{
					largest_label += 1;
					largest_label_int ++;
					label[y*result->w+x] = largest_label;
				}
				else if((left !=0) && (above ==0))
					label[y*result->w+x] = Find(left,labels);
				else if((left == 0) && (above != 0))
					label[y*result->w+x] = Find(above,labels);
				else
				{
					unionBLOB(left,above,labels);
					label[y*result->w+x] = Find(left,labels);
				}

			}
		}
	for(int y =0; y <h; y++)
		for(int x=0; x<w;x++)
		{
			pixel = get_pixel(surface,x,y);
			SDL_GetRGB(pixel,surface->format,&r,&g,&b);
			if(r==255)
				label[y*surface->w+x] = Find(label[y*surface->w+x],labels);
		}
	SDL_SaveBMP(result,"labeling.bmp");
	SDL_FreeSurface(surface);
	SDL_FreeSurface(result);
	free(labels);
	return largest_label_int;
}

struct pixelandcount {
	Uint32 pixel;
	int count;
};
void LargestComponentColor(char* image)
{
	int largest_label = BLOBdetection(image);
	SDL_Surface* surface = SDL_LoadBMP("labeling.bmp");
	if(!surface)
		errx(3,"can't load %s,%s","labeling.bmp",SDL_GetError());
	int h = surface->h;
	int w = surface->w;

	struct pixelandcount *count = (struct pixelandcount*) calloc(largest_label,sizeof(struct pixelandcount));
	int i = 0;
	for(int y = 0; y <h; y++)
		for(int x =0; x < w; x++)
		{
			Uint32 pixel = get_pixel(surface,x,y);
			if(pixel != 0)
			{
				int j =0;
				while(j<largest_label && count[j].pixel != pixel)
					j++;
				if(j == largest_label)
				{
					if(i < largest_label)
					{
						count[i].pixel = pixel;
						count[i].count = 1;
						i++;
					}
				}
				else
				{
					count[j].count += 1;
				}
			}
		}
	Uint32 maxcolor = count[0].pixel;
	int maxcount = count[0].count;
	for(int k =0; k < largest_label; k++)
		if(count[k].count > maxcount)
		{
			maxcount = count[k].count;
			maxcolor = count[k].pixel;
		}
	for(int y = 0; y <h;y++)
		for(int x =0; x<w; x++)
		{
			Uint32 pixel = get_pixel(surface,x,y);
			if(pixel != maxcolor)
				put_pixel(surface,x,y,SDL_MapRGB(surface->format,0,0,0));
			else
				put_pixel(surface,x,y,SDL_MapRGB(surface->format,255,255,255));
		}
	SDL_SaveBMP(surface,"gridisolated.bmp");
	SDL_FreeSurface(surface);
	free(count);
}

struct Coordonnates TopLeft(SDL_Surface* surface)
{
	struct Coordonnates coordonnate;
	int x = 0;
	int y = 0;
	Uint32 red = SDL_MapRGB(surface->format,255,0,0);
	Uint32 pixel = get_pixel(surface,x,y);

	while(x+1 < surface->w/2 && pixel != red)
	{
		x++;
		pixel = get_pixel(surface,x,y);
		int prevy = y;
		while(y+1 < surface->h && pixel != red)
		{
			y++;
			pixel = get_pixel(surface,x,y);
		}
		if(pixel != red)
			y = prevy;
	}
	
	coordonnate.x = x;
	coordonnate.y = y;
	return coordonnate;
}

struct Coordonnates TopRight(SDL_Surface* surface)
{
	struct Coordonnates coordonnate;
	int x = surface->w-1;
	int y = 0;
	Uint32 pixel = get_pixel(surface,x,y);
	
	Uint32 red = SDL_MapRGB(surface->format,255,0,0);

	while(x-1>0 && pixel != red)
	{
		x--;
		pixel = get_pixel(surface,x,y);
		int prevy = y;
		while(y+1 < surface->h && pixel != red)
		{
			y++;
			pixel = get_pixel(surface,x,y);
		}
		if(pixel != red)
			y = prevy;
	}
	coordonnate.x = x;
	coordonnate.y = y;
	return coordonnate;
}


struct Coordonnates BottomLeft(SDL_Surface* surface)
{
	struct Coordonnates coordonnate;
	int x = 0;
	int y = surface->h-1;
	Uint32 pixel = get_pixel(surface,x,y);
	
	Uint32 red = SDL_MapRGB(surface->format,255,0,0);

	while(x+1<surface->w && pixel != red)
	{
		x++;
		pixel = get_pixel(surface,x,y);
		int prevy = y;
		while(y-1 >0 && pixel != red)
		{
			y--;
			pixel = get_pixel(surface,x,y);
		}
		if(pixel != red)
			y = prevy;
	}
	coordonnate.x = x;
	coordonnate.y = y;
	return coordonnate;
}

struct Coordonnates BottomRight(SDL_Surface* surface)
{
	struct Coordonnates coordonnate;
	int x = surface->w-1;
	int y = surface->h-1;
	Uint32 pixel = get_pixel(surface,x,y);
	
	Uint32 red = SDL_MapRGB(surface->format,255,0,0);
	while(x-1 > 0 && pixel != red)
	{
		x--;
		pixel = get_pixel(surface,x,y);
		int prevy = y;
		while(y-1 > 0 && pixel != red)
		{
			y--;
			pixel = get_pixel(surface,x,y);
		}
		if(pixel != red)
			y = prevy;
	}
	coordonnate.x = x;
	coordonnate.y = y;
	return coordonnate;
}

void Find4cornersandcrop(char* image1,char* image2)
{
	SDL_Surface* surface =SDL_LoadBMP(image1);
	if(!surface)
		errx(3,"can't load %s, %s",image1,SDL_GetError());
	SDL_Surface* surface3 = SDL_LoadBMP(image2);
	if(!surface3)
		errx(3,"can't load %s, %s",image2,SDL_GetError());
	struct Coordonnates corners[4];
	corners[0] = TopLeft(surface);
	corners[1] = TopRight(surface);
	corners[2] = BottomLeft(surface);	
	corners[3] = BottomRight(surface);
	SDL_Surface* surface2 = SDL_CreateRGBSurface(0,corners[3].x-corners[0].x,corners[2].y-corners[0].y,32,0,0,0,0);
	
	for(int y =0;y < surface2->h; y++)
		for(int x = 0; x < surface2->w;x++)
		{
			put_pixel(surface2,x,y,(get_pixel(surface3,corners[0].x+x,corners[0].y+y)));
		}
	SDL_SaveBMP(surface2,"croppedgrid.bmp");
	SDL_FreeSurface(surface);
	SDL_FreeSurface(surface2);
	SDL_FreeSurface(surface3);
}
void SaveNewImage(SDL_Surface* surface,int j, int i, int hbox, int wbox)
{
	SDL_Surface* box = SDL_CreateRGBSurface(0,wbox,hbox,32,0,0,0,0);
	for(int y = i*hbox; y < (i+1)*hbox;y++)
		for(int x = j*wbox; x< (j+1)*wbox;x++)
			put_pixel(box,x-j*wbox,y-i*hbox,(get_pixel(surface,x,y)));
	char name[] = "Grid_0x0.bmp";
	name[5] =(char) (i +48);
	name[7] = (char) (j+48);
	SDL_SaveBMP(box,name);
	SDL_FreeSurface(box);
}

void CutImage(char* image)
{
	SDL_Surface* surface =SDL_LoadBMP(image);
	if(!surface)
		errx(3,"can't load %s,%s",image,SDL_GetError());
	int h = surface->h;
	int w = surface->w;
	int hbox = h/9;
	int wbox = w/9;

	for(int i = 0;i <= 8; i++)
		for(int j = 0;j <= 8;j++)
		{
			SaveNewImage(surface,j,i,hbox,wbox);
		}
	SDL_FreeSurface(surface);
}

void cut_grid(char* image)
{
	blackandwhiteinversion(image);
    	Dilatation("inversion.bmp","dilatation.bmp");
    	SobelFilter("dilatation.bmp","Sobel.bmp");
    	int angle = HoughTransformation2("Sobel.bmp","hough1.bmp",300);
	angle = angle - 90;
	rotateauto(image,angle,"rotatedImage.bmp");
	rotateauto("inversion.bmp",angle,"rotatedInversion.bmp");
	LargestComponentColor("rotatedInversion.bmp");
	HoughTransformation("gridisolated.bmp","hough2.bmp");
    	Find4cornersandcrop("hough2.bmp", "rotatedImage.bmp");
    	CutImage("croppedgrid.bmp");

}
