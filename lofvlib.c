#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lofvlib.h"
void lofv_load(char *fname, lofv **lofv1)
{
  int lines,i;
  char s[40];
  float f;
  FILE *flofv;
  printf("Loading lines of vision file %s ... ", fname); fflush(stdout);
  if(!(flofv=fopen(fname,"r"))){ fprintf(stderr, "\n error loading lines of vision"); exit(1);}
  fscanf(flofv, "%s", s);
  while((strncmp("lines_of_vision",s,40)!=0)&&(!feof(flofv))) fscanf(flofv, "%s", s);
  fscanf(flofv, "%d", &lines);
  (*lofv1)=(lofv *)malloc(sizeof(lofv));
  (*lofv1)->n=lines;
  (*lofv1)->lines=(line *)malloc(sizeof(line)*lines);

  for(i=0; i<lines; i++)
  {
      fscanf(flofv, "%f", &f);  (*lofv1)->lines[i].v1.x=f;
      fscanf(flofv, "%f", &f);  (*lofv1)->lines[i].v1.y=f;
      fscanf(flofv, "%f", &f);  (*lofv1)->lines[i].v1.z=f;

      fscanf(flofv, "%f", &f);  (*lofv1)->lines[i].v2.x=f;
      fscanf(flofv, "%f", &f);  (*lofv1)->lines[i].v2.y=f;
      fscanf(flofv, "%f", &f);  (*lofv1)->lines[i].v2.z=f;
  }

  printf("DONE\n"); fflush(stdout);
  fclose(flofv);
}

