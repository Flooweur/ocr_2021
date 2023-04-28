#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{

	char filename[40];

	printf("Enter filename : ");
	scanf("%s", filename);

	/* On prepare nom du fichier de destination */
	size_t size = strlen(filename)+1;
	char *bmpfilename = (char *) malloc(size);
	strcpy(bmpfilename, filename);

	/* On cree une copie du nom du fichier mais on change son extension */
	char *point = strrchr(bmpfilename, '.');
	strcpy(point, ".bmp");

	/* Enfin, on renomme le fichier original. */
	rename(filename, bmpfilename);


	return 0;
}

