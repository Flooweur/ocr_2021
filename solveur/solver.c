#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "solver.h"

char grid[9][9];

/*----------Read&Write----------*/
void setgrid(char *filename)
{
	FILE *f;
	f = fopen(filename, "r");
	int ch = fgetc(f);
	int i = 0; int j = 0;//compteur pour la grille
	while(ch != EOF)
	{
		if (ch == '.') 
			grid[j][i] = 0;
		else if (ch > '0' && ch <= '9') 
			grid[j][i] = ch - '0';
		if (ch == ' ' || ch == 10) 
			i--;
		
		if (i == 9)
		{
			i = 0;
			j++;
		}
		i++;
		ch = fgetc(f);
	}
	fclose(f);
}

void writegrid(char *filename)
{
	FILE *f;
	char *name = malloc(strlen(filename) + strlen(".result") + 1);
	strcpy(name, filename);
	strcat(name, ".result");	
	f = fopen(name, "w");
	for(size_t i = 0; i < 9; i++)
	{
		if (i == 3 || i == 6) fputc('\n', f);
		for (size_t j = 0; j < 9; j++)
		{
			if(j == 3 || j == 6) fputc(' ', f);
			fputc(grid[i][j]+'0', f);
			if(j == 8 && i != 8) fputc('\n', f);
		}
	}
	fclose(f);
}

/*----------MAIN----------*/
int main(int argc, char **argv)
{
	if (argc < 2) return 1;
	setgrid(argv[1]);
	printg();
	if(!issolvable())
	{
		printf("The grid is not valid\n");
		return 1;
	}
	solve();
	int b = 1;
	for (int x = 0; x < 9; x++)
		b = b && (!(iscolumnsolved(x)&& islinesolved(x)));
	if (b) 
	{
		printf("The grid is not solvable\n");
		return 1;
	}
	printg();
	writegrid(argv[1]);
	return 0;
}

/*----------ToRemove----------*/
void printg()
{
	printf("-------------------------------\n");
	for (short i = 0; i < 9; i++)
	{
		printf("|");
		for (short j = 0; j < 9; j++)
		{
			char c = grid[i][j];
			if (c != 0) printf(" %hhi ", c);
			else printf("   ");
			if(j%3 == 2) printf("|");
		
		}
		printf("\n");
		if(i%3 == 2) printf("-------------------------------\n");
	}
}

char issolvable()
{
    for (short i = 0; i < 9; i++)
	for (short j = 0; j < 9; j++)
    	    if (grid[i][j] != 0)
	    {
		char x = grid[i][j];
		grid[i][j] = 0;
		if ((alreadyincolumn(i, x) || alreadyinline(j, x) || alreadyinsquare(i, j, x)))
		    return 0;
		grid[i][j] = x;	
	    }
    return 1;
}

/*----------solveur----------*/
char iscolumnsolved(short x)
{
	for (short i = 0; i < 9; i++)
	{
		char t = grid[x][i];
		if (t == 0) return 0;
		for (short j = i+1; j < 9; j++)
			if (t == grid[x][j]) return 0; 
	}
	return 1;
}


char islinesolved(short x)
{
	for (short i = 0; i < 9; i++)
	{
		char t = grid[i][x];
		if (t == 0) return 0;
		for (short j = i+1; j < 9; j++)
			if (t == grid[j][x]) return 0; 
	}
	return 1;
}

char issquaresolved(short x, short y)
{
	x -= x%3;
	y -= y%3;
	for (short i = x; i < x + 3; i++)
	{
		for (short j = y; j < y + 3; j++)
		{
			char t = grid[i][j];
			if (t == 0) return 0;
			for (short k = x; k < x + 3; k++)
				for (short l = y; l < y+3; l++)
					if ((k != i || l != j) && t == grid[k][l])
						return 0;
		}
	}

	return 1;
}

char alreadyincolumn(short x, char val)
{
	for (short i = 0; i < 9; i++)
		if (val == grid[x][i]) return 1;
	return 0;
}

char alreadyinline(short x, char val)
{
	for (short i = 0; i < 9; i++)
		if (val == grid[i][x]) return 1;
	return 0;
}

char alreadyinsquare(short x, short y, char val)
{
	x -= x%3;
	y -= y%3;
	for (short i = x; i < x + 3; i++)
		for (short j = y; j < y + 3; j++)
			if (val == grid[i][j]) return 1;
	return 0;
}

char solverec(short x, short y)
{
	char p = x * 9 + y;

	if (x == 9)return 1;
	if (grid[x][y] != 0)
		return solverec((p+1)/9, (p+1)%9);
	for (short i = 1; i < 10; i++)
		if(!(alreadyincolumn(x,i) || alreadyinline(y, i) || alreadyinsquare(x, y, i)))
		{
			grid[x][y] = i;
			if(solverec( (p+1)/9, (p+1)%9))
				return 1;
		}
	grid[x][y] = 0;
	return 0;
}

void solve()
{
	solverec(0,0);
}
