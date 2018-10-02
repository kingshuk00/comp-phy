/******************************************************************************
    error.c is a way to explore truncation and round-off error in computation
    Copyright (C) 2018  Kingshuk

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

 *****************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<float.h>
#include<tgmath.h>

#ifdef DOUBLE
# define real double
const double _eps= DBL_EPSILON;
#else /* DOUBLE */
# define real float
const double _eps= FLT_EPSILON;
#endif /* !DOUBLE */

#define MIN(x,y) x<y?x:y

/* 0.1*x**4 - 0.15*x**3 - 0.5*x**2 - 0.25*x+ 1.2 */
real f(const real x)
{
#if 1
   real result= 0.1* x* x* x* x- 0.15* x* x* x- 0.5* x* x- 0.25* x+ 1.2;
#else
   real result= 0.1* pow(x, 4.0)- 0.15* pow(x, 3.0)- 0.5* pow(x, 2.0)-
      0.25* x+ 1.2;
#endif
   return result;
}

real f1(const real x)
{
#if 1
   real result= 0.4* x* x* x- 0.45* x* x- 1.0* x- 0.25;
#else
   real result= 0.4* pow(x, 3.0)- 0.45* pow(x, 2.0)- 1.0* x- 0.25;
#endif
}

real f3(const real x)
{
   real result= 2.4* x- 0.9;
   return result;
}

const real *getInterval(const real x, const real h)
{
   static real interval[2];
   interval[0]= x- h;
   interval[1]= x+ h;
   return interval;
}

real getOptStepSize(const real x, const int start, const int end)
{
   const double epsilon= ((double) x)* _eps;
   const double M= f3(x+ pow(10.0, ((double) end)));
   const real oneByThree= 1.0/ 3.0;
   const real result= (real) (pow(3.0* epsilon/ M, oneByThree));
   return result;
}

void printError(const int start, const int end,
                const real *const ef,
                const real *const eb,
                const real *const ec)
{
   
   printf(" ---------------------------------------------------------------\n");
   printf(" | step-size | error-forward | error-backward | error-centered |\n");
   printf(" ---------------------------------------------------------------\n");
   for(int i= start; i< end; ++i) {
      const int index= i- start;
      printf(" | %9d |  %.6e |   %.6e |   %.6e |\n", i, ef[index], eb[index],
             ec[index]);
   }
   printf(" ---------------------------------------------------------------\n");
}

void gnuplotError(const int start, const int end, const real x,
                  const real *const ef,
                  const real *const eb,
                  const real *const ec)
{
   FILE *gp= popen("gnuplot", "w");
   if(!gp) { return; }
   fprintf(gp, "set term png medium size 640,480\n");
   {
      char fn[64]= { '\0' };
      sprintf(fn, "%g.png", x);
      fprintf(gp, "set output \'%s\'\n", fn);
   }
   fprintf(gp, "set logscale y 10\n");
   fprintf(gp, "p \'-\' w lp lc rgb \'red\' title \"centered\"");
   fprintf(gp, ", \'-\' w lp title \"forward\"");
   fprintf(gp, ", \'-\' w lp title \"backward\"");
   fprintf(gp, "\n");

   for(int i= start; i< end; ++i) {
      fprintf(gp, "%d %.6e\n", i, ec[i- start]);
   }
   fprintf(gp, "e\n");
   for(int i= start; i< end; ++i) {
      fprintf(gp, "%d %.6e\n", i, ef[i- start]);
   }
   fprintf(gp, "e\n");
   for(int i= start; i< end; ++i) {
      fprintf(gp, "%d %.6e\n", i, eb[i- start]);
   }
   fprintf(gp, "e\n");

   pclose(gp);
   gp= NULL;
}


void perform(const real x)
{
   const int start= -14;
   const int end= 1;
   int num= end- start;
   real *ef= malloc(sizeof(real)* num);
   real *eb= malloc(sizeof(real)* num);
   real *ec= malloc(sizeof(real)* num);

   real optStep= getOptStepSize(x, start, end);
   printf("x= %.6e, optimal step size= %.6e\n", x, optStep);
   double stepDiff= DBL_MAX;
   int iStep;
   for(int i= start; i< end; ++i) {
      const real h= pow(10.0, ((real) i));
      const real *interval= getInterval(x, h);
      const real centered= (f(interval[1])- f(interval[0]))/ (2* h);
      const real forward= (f(interval[1])- f(x))/ h;
      const real backward= (f(x)- f(interval[0]))/ h;
      const real actual= f1(x);
      ef[i- start]= fabs(actual- forward);
      eb[i- start]= fabs(actual- backward);
      ec[i- start]= fabs(actual- centered);
      stepDiff= MIN(stepDiff,ec[i-start]);
      if(stepDiff== ec[i- start]) {
         iStep= i;
      }
   }
   printf("Best result at 10^%d\n", iStep);
   printError(start, end, ef, eb, ec);
   gnuplotError(start, end, x, ef, eb, ec);
}

int main(int argc, char *argv[])
{
   if(8== sizeof(real)) {
      printf("Using double precision\n");
   } else if(4== sizeof(real)) {
      printf("Using single precision\n");
   } else {
      printf("Using unknown precision\n");
   }
   for(int i= 1; i< argc; ++i) {
      real x= (real) atof(argv[i]);
      perform(x);
   }
   return 0;
}
