#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "colors.h"
#include "griddetection.h"
#include "testexport.h"
#include "neuNetwork.h"
#include <err.h>
#include "SDL/SDL.h"

int main(int argc, char** argv)
{
	if(argc != 2)
	{
	        errx(1, "Main must be called with 1 arguments: str");
	}
	char* path = argv[1];
	char* image = imagetobmp(path);
	image_togray(image);
	image_toblackandwhite("grayscale.bmp");
    cut_grid("blackandwhite.bmp");
    int *listToSolve = exportMNIST();
    int *listFilled = NeuralNetwork(listToSolve);
    saveToFormat(listFilled);
    return 0;
}
