/******************************************************************************
    open.c is to explore fixed point iteration & Newton Raphson methods
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
#include<string.h>
#include<tgmath.h>

#ifdef DOUBLE
# define real double
#else /* DOUBLE */
# define real float
#endif /* !DOUBLE */

#define MAX(x,y) x>y?x:y
#define MIN(x,y) x<y?x:y

real f(real x)
{
   real result= 3* x* x* x+ x- 3;
   return result;
}

real f1(real x)
{
   real result= 9* x* x+ 1;
}

real g(real x)
{
   real result= pow((3.0- x)/ 3.0, 1.0/3.0);
   return result;
}

real g1(real x)
{
   real result= -1.0* pow((3.0- x)/ 3, -2.0/3.0)/ 9.0;
   return result;
}

real fixedPoint(const real x)
{
   real result= g(x);
   if(g1(x)>= 1.0) {
      printf(" |  < **may not converge** > |\n");
   }
   return result;
}

real NewtonRaphson(const real x)
{
   real result= x- (f(x)/ f1(x));
   return result;
}

real *openIteration(int *const nump,
                    const real precision,
                    const real xinit,
                    real (*next)(const real))
{
   const int num= *nump;
   real x= xinit;
   real *error= malloc(sizeof(real)* num);
   memset(error, 0, sizeof(real)* num);

   printf(" ----------------------------------------------\n");
   printf(" | #iteration |       x       |     error     |\n");
   printf(" ----------------------------------------------\n");
   printf("            0 | %.7e | %.7e |\n", x, x);
   int i;
   for(i= 0; i< num; ++i) {
      real new_x= next(x);
      error[i]= (real) (fabs(x- new_x)/ new_x);
      x= new_x;
      if(error[i]< precision) {
         printf(" |    **Break: Reached desired precision**    |\n");
         break;
      }
      printf(" | %10d | %.7e | %.7e |\n", i+ 1, x, error[i]);
   }
   printf(" ----------------------------------------------\n\n");
   *nump= i;
   return error;
}

void gnuplotError(const int fpNum,
                  const real *const fpError,
                  const int nrNum,
                  const real *const nrError)
{
   FILE *gp= popen("gnuplot", "w");
   if(!gp) { return; }
   fprintf(gp, "set term png medium size 640,480\n");
   fprintf(gp, "set output \'open.png\'\n");
   fprintf(gp, "set logscale y 10\n");
   fprintf(gp, "set grid\n");
   fprintf(gp, "set ylabel \"Error\"\n");
   fprintf(gp, "set xlabel \"#iteration\"\n");
   fprintf(gp, "p \'-\' w lp title \"Fixed-point\"");
   fprintf(gp, ", \'-\' w lp title \"Newton-Raphson\"");
   fprintf(gp, "\n");

   for(int i= 0; i< fpNum; ++i) {
      fprintf(gp, "%d %.6e\n", i, fpError[i]);
   }
   fprintf(gp, "e\n");
   for(int i= 0; i< nrNum; ++i) {
      fprintf(gp, "%d %.6e\n", i, nrError[i]);
   }
   fprintf(gp, "e\n");

   pclose(gp);
   gp= NULL;
}

int main(int argc, char *argv[])
{
   if(argc< 3) { return 1; }
   real xinit= (real)  atof(argv[1]);
   real precision= (real) (atof(argv[2])/ 100.0);

   /* fixed-point iteration */
   printf(" Fixed-point:\n");
   int fpNum= 100;
   real *fpError= openIteration(&fpNum, precision, xinit, &fixedPoint);

   /* Newton-Raphson iteration */
   int nrNum= 100;
   real *nrError= openIteration(&nrNum, precision, xinit, &NewtonRaphson);

   gnuplotError(fpNum, fpError, nrNum, nrError);

   if(fpError) {
      free(fpError); fpError= NULL;
   }
   if(nrError) {
      free(nrError); nrError= NULL;
   }

   return 0;
}
