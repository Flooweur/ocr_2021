#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_rotozoom.h"
#include "pixel_operations.h"
#include <SDL/SDL_video.h>
#include <err.h>

void writeinval(int dec, char *f)
{
    FILE *file;
    file = fopen(f, "a");
    char *a = (char*)malloc(sizeof(char) * 3);
    char c;
    int slice = dec;
    int remain;
    int i = 0;
    while (slice > 0){
        remain = slice % 16;
        slice = slice / 16;
        switch (remain){
            case 10 : c = 'a'; break;
            case 11 : c = 'b'; break;
            case 12 : c = 'c'; break;
            case 13 : c = 'd'; break;
            case 14 : c = 'e'; break;
            case 15 : c = 'f'; break;
            default : c = remain + '0';
        }
        a[i] = c;
        i++;
    }
    a[i] = '\0';
    /*a[8] = '\0';
    for (int i = 0; i < 8; i++)
    {
        a[8 - i] = '0' + dec%2;
        dec = dec/2;
    }
    unsigned char val = strtol(a, NULL, 2);*/
    for(int j = 0; j >= 0; j--)
    {
        unsigned char val = strtol(&a[j], NULL, 16);
        fwrite(&val, sizeof(unsigned char), 1, file);
    }
    fclose(file);
    free(a);
}

SDL_Surface *cropSurface(SDL_Surface *surface)
{
    int wsurface = ((surface->w)/5);
    int hsurface = ((surface->h)/5);
    SDL_Surface* cropped = SDL_CreateRGBSurface(0,3*wsurface,3*hsurface,32,0,0,0,0);
	for(int y = hsurface; y < 4*hsurface;y++)
		for(int x = wsurface; x< 4*wsurface;x++)
			put_pixel(cropped,x-wsurface,y-hsurface,(get_pixel(surface,x,y)));
    return cropped;
}

uint moyenne(SDL_Surface *surface)
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

int* exportMNIST()
{
    static int gridToSolve[81];
    writeinval(0,"gridexport");
    writeinval(0,"gridexport");
    writeinval(8,"gridexport");
    writeinval(3,"gridexport");
    writeinval(0,"gridexport");
    writeinval(0,"gridexport");
    writeinval(0,"gridexport");
    writeinval(81,"gridexport");
    writeinval(0,"gridexport");
    writeinval(0,"gridexport");
    writeinval(0,"gridexport");
    writeinval(28,"gridexport");
    writeinval(0,"gridexport");
    writeinval(0,"gridexport");
    writeinval(0,"gridexport");
    writeinval(28,"gridexport");
/*
	v = strtol("00000000", NULL, 2);
	fwrite(&v, sizeof(v), 1, file);
	fwrite(&v, sizeof(v), 1, file);
	*v = strtol("00000001", NULL, 2);
	fwrite(&v, sizeof(v), 1, file);
	*v = strtol("00000011", NULL, 2);
	fwrite(&v, sizeof(v), 1, file);

	*v = strtol("00000000", NULL, 2);
	fwrite(&v, sizeof(v), 1, file);
	fwrite(&v, sizeof(v), 1, file);
	*v = strtol("00000101", NULL, 2);
	fwrite(&v, sizeof(v), 1, file);
	*v = strtol("00010000", NULL, 2);
	fwrite(&v, sizeof(v), 1, file);

	*v = strtol("00000000", NULL, 2);
	fwrite(&v, sizeof(v), 1, file);
	fwrite(&v, sizeof(v), 1, file);
	*v = strtol("00000001", NULL, 2);
	fwrite(&v, sizeof(v), 1, file);
	*v = strtol("11000000", NULL, 2);
	fwrite(&v, sizeof(v), 1, file);

	*v = strtol("00000000", NULL, 2);
	fwrite(&v, sizeof(v), 1, file);
	fwrite(&v, sizeof(v), 1, file);
	*v = strtol("00000001", NULL, 2);
	fwrite(&v, sizeof(v), 1, file);
	*v = strtol("11000000", NULL, 2);
	fwrite(&v, sizeof(v), 1, file);
*/
	char boxname[] = "Grid_xxy.bmp";
	//char hex[33];
	//parcours de toutes les images des cases
	for (size_t x = 0; x < 9; x++)
	{
		for (size_t y = 0; y < 9; y++)
		{

			//stupid thing
			boxname[5] = x==1 ? '1' : x==2 ? '2' : x==3 ? '3' : x==4 ? '4' : x==5 ? '5' : x==6 ? '6' : x==7 ? '7' : x==8 ? '8' : '0';
			boxname[7] = y==1 ? '1' : y==2 ? '2' : y==3 ? '3' : y==4 ? '4' : y==5 ? '5' : y==6 ? '6' : y==7 ? '7' : y==8 ? '8' : '0';

			SDL_Surface* box0 = cropSurface(SDL_LoadBMP(boxname));
			SDL_Surface* box = zoomSurface(box0, (double)28/(box0->w), (double)28/(box0->h), 1);
			if (!box)
				errx(1, "failed to load image %s \n", boxname);
			else
            {
                if(moyenne(box0) > 245)
                    gridToSolve[x*9 + y] = 0;
                else
                    gridToSolve[x*9 + y] = 9;
			//parcours de tous les pixels
			    for (int i = 0; i < box->w; i++)
			    {
				    for (int j = 0; j < box->h; j++)
				    {
					    Uint8 red, green, blue;
					    Uint32 pixel = get_pixel(box, j, i);
					    SDL_GetRGB(pixel,box->format, &red, &green, &blue);
					    if (red > 127)
						    writeinval(0,"gridexport");
						    //hex[l] = '0';
					    else
						    writeinval(255,"gridexport");
					    	//hex[l] = '1';
				    }
			    }
            }
			SDL_FreeSurface(box0);
			SDL_FreeSurface(box);
		}
	}
    return gridToSolve;
}

void saveToFormat(int* listFilled)
{
    FILE *file;
    file = fopen("gridToSolve", "w");
    int i=0, j=0;
    while(i < 9)
    {
        j = 0;
        if(i == 3 || i == 6)
            fwrite("\n", sizeof(char), 1, file);
        while(j < 9)
        {
            if(j == 3 || j == 6)
                fwrite(" ", sizeof(char), 1, file);
            if(listFilled[i*9 + j] == 0)
                fwrite(".", sizeof(char), 1, file);
            if(listFilled[i*9 + j] == 1)
                fwrite("1", sizeof(char), 1, file);
            if(listFilled[i*9 + j] == 2)
                fwrite("2", sizeof(char), 1, file);
            if(listFilled[i*9 + j] == 3)
                fwrite("3", sizeof(char), 1, file);
            if(listFilled[i*9 + j] == 4)
                fwrite("4", sizeof(char), 1, file);
            if(listFilled[i*9 + j] == 5)
                fwrite("5", sizeof(char), 1, file);
            if(listFilled[i*9 + j] == 6)
                fwrite("6", sizeof(char), 1, file);
            if(listFilled[i*9 + j] == 7)
                fwrite("7", sizeof(char), 1, file);
            if(listFilled[i*9 + j] == 8)
                fwrite("8", sizeof(char), 1, file);
            j++;
        }
        fwrite("\n", sizeof(char), 1, file);
        i++;
    }
    fclose(file);
}
