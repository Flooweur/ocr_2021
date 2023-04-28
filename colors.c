#include "SDL/SDL.h"
#include <stdio.h>
#include <err.h>
#include "SDL/SDL_image.h"
#include "pixel_operations.h"
#include <math.h>
#include <SDL/SDL_rotozoom.h>


char* imagetobmp(char* image)
{
	SDL_Surface* surface = IMG_Load(image);
	if(!surface)
		errx(3,"can't load %s:%s",image, SDL_GetError());
	SDL_SaveBMP(surface,"inputgrid.bmp");
	SDL_FreeSurface(surface);
	return "inputgrid.bmp";
}

void image_togray(char* image)
{
	SDL_Surface* surface = SDL_LoadBMP(image);
	if(!surface)
	{
		errx(3,"can't load %s: %s",image, SDL_GetError());
	}
	SDL_Surface* surface_copy = SDL_CreateRGBSurface(0,surface->w,surface->h,32,0,0,0,0);	
	Uint32 pixel;
	Uint8 r,g ,b, gray;
	for (int x= 0;  x < surface->w; x++)
	{
		for(int y = 0; y <surface->h; y++)
		{
			pixel = get_pixel(surface,x,y);
			SDL_GetRGB(pixel,surface->format,&r,&g,&b);
			gray = 0.3*r + 0.59*g+0.11*b;
			pixel = SDL_MapRGB(surface->format,gray,gray,gray);
			put_pixel(surface_copy,x,y,pixel);
		}

	}
	SDL_SaveBMP(surface_copy, "grayscale.bmp");
	SDL_FreeSurface(surface);
	SDL_FreeSurface(surface_copy);
}

/* fonction that calculates the minimum image intensities within a local neighborhood
 * window = 3 * 3 square window
 */
SDL_Surface* Gaussian(SDL_Surface* surface, int radius)
{
	SDL_Surface* newSurface = surface;
	Uint32 pixel;
        Uint8 r,g,b;
	int loop = (radius - 1)/2;
	int sum = 0;
	for(int x = loop ; x < surface->w-loop; x++)
        {
                for(int y = loop; y < surface->h-loop;y++)
                {	
			for(int i = x - loop ; i < x + loop + 1; i++)              
       			{
	                	for(int j = y - loop; j < y + loop + 1;j++)
                		{
					pixel = get_pixel(surface,i,j);
					SDL_GetRGB(pixel,surface->format,&r,&g,&b);
					if (i != x && y != j)
						sum += r;
				}
			}
			sum = sum/((radius * radius) - 1);
			pixel = SDL_MapRGB(surface->format,sum,sum,sum);
			put_pixel(newSurface,x,y,pixel);
                }
        }
	return newSurface;

}

SDL_Surface* Dilate(SDL_Surface* surface)
{
        //SDL_Surface* newSurface = surface;
        SDL_Surface* newSurface = SDL_CreateRGBSurface(0,surface->w,surface->h,32,255,255,255,0);
	Uint32 pixel;
        Uint8 r,g,b;
	Uint32 npixel = SDL_MapRGB(surface->format,0,0,0);
	for(int x = 1 ; x < surface->w-1; x++)
        {
                for(int y = 1; y < surface->h-1;y++)
                {
			pixel = get_pixel(surface,x,y);
                        SDL_GetRGB(pixel,newSurface->format,&r,&g,&b);
			if (r == 0)
			{
				npixel = SDL_MapRGB(surface->format,0,0,0);
        			put_pixel(newSurface,x,y,npixel);
				put_pixel(newSurface,x-1,y-1,npixel);
				put_pixel(newSurface,x,y-1,npixel);
				put_pixel(newSurface,x+1,y-1,npixel);
				put_pixel(newSurface,x-1,y,npixel);
				put_pixel(newSurface,x+1,y,npixel);
				put_pixel(newSurface,x-1,y+1,npixel);
				put_pixel(newSurface,x,y+1,npixel);
				put_pixel(newSurface,x+1,y+1,npixel);
                        }
			else
			{
				npixel = SDL_MapRGB(surface->format,255,255,255);
                                put_pixel(newSurface,x,y,npixel);
			}
                }
        }
        return newSurface;

}

SDL_Surface* Binar(SDL_Surface* surface, int w, int h, int t)
{
    uint sum;
	Uint8 grad;
	float *intImg[w];
	int s = w/32;
	float max = 0;
	Uint32 pixel;
	for(int i = 0; i < w; i++)
	{
		sum = 0;
		intImg[i] = (float*)malloc(h * w * sizeof(float));
		for(int j = 0; j < h; j++)
		{
			pixel = get_pixel(surface,i,j);
                        SDL_GetRGB(pixel,surface->format,&grad,&grad,&grad);
			sum += grad;
			if(i == 0)
				intImg[i][j] = sum;
			else
				intImg[i][j] = intImg[i-1][j] + sum;
			if(intImg[i][j] > max)
				max = intImg[i][j];
		}
	}
	int x1, x2, y1, y2;
	float count;
	SDL_Surface* out = SDL_CreateRGBSurface(0,surface->w,surface->h,32,0,0,0,0);
	for(int i = 0; i < w; i++)
	{
		for(int j = 0; j < h; j++)
		{
			if(i - s/2 < 0)
				x1 = 1;
			else
				x1 = i - s/2;
			if(i + s/2 > w - 1)
                x2 = w - 1;
            else
                x2 = i + s/2;
			if(j - s/2 < 0)
                y1 = 1;
            else
                y1 = j - s/2;
			if(i + s/2 > h - 1)
                y2 = h - 1;
            else
                y2 = j + s/2;
            count = (x2 - x1) * (y2 - y1);
			sum = intImg[x2][y2] - intImg[x2][y1-1] - intImg[x1-1][y2] + intImg[x1-1][y1-1];
			pixel = get_pixel(surface,i,j);
                        SDL_GetRGB(pixel,surface->format,&grad,&grad,&grad);
			if ((grad * count) <= (sum * (100-t)/100))
			{
				pixel = SDL_MapRGB(surface->format,0,0,0);
                                put_pixel(out,i,j,pixel);
			}
			else
			{
				pixel = SDL_MapRGB(surface->format,255,255,255);
                                put_pixel(out,i,j,pixel);
			}
		}
		free(intImg[i]);
	}
	return out;
}

uint mean(SDL_Surface *surface)
{
	Uint8 grad;
        Uint32 pixel;
	uint sum = 0;
        for(int x = 0; x < surface->w - 1; x++)
        {
                for(int y = 0; y < surface->h - 1; y++)
                {
                        pixel = get_pixel(surface,x,y);
                        SDL_GetRGB(pixel,surface->format,&grad,&grad,&grad);
			sum += grad;
		}
	}
	return sum/(surface->h * surface->w);
}

SDL_Surface* Compare(SDL_Surface *surface1, SDL_Surface *surface2)
{
	SDL_Surface* surface_copy = SDL_CreateRGBSurface(0,surface1->w,surface1->h,32,0,0,0,0);
	Uint8 grad1, grad2;
	Uint32 pixel1, pixel2;
	for(int x = 0; x < surface1->w - 1; x++)
	{
		for(int y = 0; y < surface1->h - 1; y++)
		{
			pixel1 = get_pixel(surface1,x,y);
                        SDL_GetRGB(pixel1,surface1->format,&grad1,&grad1,&grad1);
			pixel2 = get_pixel(surface2,x,y);
                        SDL_GetRGB(pixel2,surface2->format,&grad2,&grad2,&grad2);
			if (grad1 == 0 && grad2 == 0)
			{
                pixel1 = SDL_MapRGB(surface1->format,0,0,0);
                put_pixel(surface_copy,x,y,pixel1);
            }
            else
            {
                pixel1 = SDL_MapRGB(surface1->format,255,255,255);
                put_pixel(surface_copy,x,y,pixel1);
            }
		}
	}
	return surface_copy;
}

Uint8 localminima_maxima(SDL_Surface* surface, int x, int y)
{
	Uint32 pixel = get_pixel(surface,x,y);
	Uint8 r,g,b;
	SDL_GetRGB(pixel,surface->format,&r,&g, &b);
	int i = x-2;
	int j = y-2;
	double R = 128;
	double k = 0.20;
	double sum = 0;
	while(i < x + 3 && i< surface->w )
	{
		while(j < y + 3 && j < surface->h)
		{
			pixel = get_pixel(surface,i,j);
			SDL_GetRGB(pixel,surface->format,&r,&g, &b);
			sum += r;
			j+=1;
		}
		j= y-2;
		i += 1;
	}
	i = x-2;
        j = y-2;
	double mean = sum/25;
	double s = 0;
	while(i < x + 3 && i< surface->w )
        {
                while(j < y + 3 && j < surface->h)
                {
                        pixel = get_pixel(surface,i,j);
                        SDL_GetRGB(pixel,surface->format,&r,&g, &b);
			s += (r - mean)*(r - mean);
                        j+=1;
                }
                j= y-2;
                i += 1;
        }
	double s2 = s/25;
	double s3 = sqrt(s2);
	return mean * (1 + k *((s3 / R) - 1));
}
void image_toblackandwhite(char*image)
{
	SDL_Surface* surface = SDL_LoadBMP(image);
	SDL_Surface* surface_copy = Binar(surface, surface->w, surface->h, 15);
	if (mean(surface_copy) < 200)
	{
		surface_copy = Binar(surface, surface->w, surface->h, 60);
	}
	surface_copy = Dilate(surface_copy);
	SDL_SaveBMP(surface_copy, "blackandwhite.bmp");
	SDL_FreeSurface(surface);
	SDL_FreeSurface(surface_copy);
}

void rotate(char* image, double angle)
{
    SDL_Surface* surface = SDL_LoadBMP(image);
    SDL_SaveBMP(rotozoomSurface(surface, angle, 1, 0), "rotated.bmp");
    SDL_Surface* surfaceRot = SDL_LoadBMP("rotated.bmp");
    int x1 = (sin(0.01745329252 * angle) * surface->h)/2;
    int x2 = surfaceRot->w - x1;
    int y1 = (sin(0.01745329252 * angle) * surface->w)/2;
    int y2 = surfaceRot->h - y1;
	SDL_Surface* surfaceCropped = SDL_CreateRGBSurface(0,x2-x1,y2-y1,32,0,0,0,0);
	for(int y =0;y < surfaceCropped->h; y++)
		for(int x = 0; x < surfaceCropped->w;x++)
		{
			put_pixel(surfaceCropped,x,y,(get_pixel(surfaceRot,x1+x,y1+y)));
		}
    SDL_SaveBMP(surfaceCropped, "rotatedCropped.bmp");
}
