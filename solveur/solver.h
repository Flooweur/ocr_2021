#ifndef SOLVEUR_H
#define SOLVEUR_H

char issolvable();

char iscolumnsolved(short);
char islinesolved(short);
char issquaresolved(short, short);

char alreadyincolumn(short, char);
char alreadyinline(short, char);
char alreadyinsquare(short, short, char);

char solverec(short, short);
void solve();

void printg();		

#endif
