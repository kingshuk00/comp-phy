/******************************************************************************
    bracketing.c is to explore bisection and false position method
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
#include<float.h>

#ifdef DOUBLE
# define real double
const double REAL_MAX= DBL_MAX;
#else /* DOUBLE */
# define real float
const float REAL_MAX= FLT_MAX;
#endif /* !DOUBLE */

#define MAX(x,y) x>y?x:y
#define MIN(x,y) x<y?x:y

real f(real x)
{
   real result= 3* x* x* x+ x- 3;
   return result;
}

real bisection(const real xl, const real xu)
{
   return (xl+ xu)/ 2.0;
}

real falsePosition(const real xl, const real xu)
{
   return xu- (f(xu)* (xl- xu))/ (f(xl)- f(xu));
}

real *bracketIteration(int *const nump,
                       const real precision,
                       real xl,
                       real xu,
                       real (*calc)(const real, const real))
{
   if(xl== xu) { return NULL; }
   const int num= *nump;
   real *error= malloc(sizeof(real)* num);
   memset(error, 0, sizeof(real)* num);
   real xr= (real) fabs(xu- xl);
   int i;
   printf(" ----------------------------------------------\n");
   printf(" | #iteration |       x       |     error     |\n");
   printf(" ----------------------------------------------\n");
   printf("            0 | %.7e | %.7e |\n", xr, xr);
   for(i= 0; i< num; ++i) {
      real new_xr= calc(xl, xu);
      error[i]= (real) fabs(xr- new_xr);
      xr= new_xr;
      if(error[i]< precision) {
         printf(" |    **Break: Reached desired precision**    |\n");
         break;
      }
      real product= f(xl)* f(xr);
      if(product> 0.0) {
         xl= xr;
      } else if(product< 0.0) {
         xu= xr;
      } else {
         xl= xr;
         xu= xr;
         printf(" | **Breaking as reached maximum precision**  |\n");
         break;
      }
      printf(" | %10d | %.7e | %.7e |\n", i+ 1, xr, error[i]);
   }
   printf(" ----------------------------------------------\n\n");
   *nump= i;
   return error;
}

void gnuplotError(const int bsNum,
                  const real *const bsError,
                  const int fpNum,
                  const real *const fpError)
{
   FILE *gp= popen("gnuplot", "w");
   if(!gp) { return; }
   fprintf(gp, "set term png medium size 640,480\n");
   fprintf(gp, "set output \'bracketing.png\'\n");
   fprintf(gp, "set logscale y 10\n");
   fprintf(gp, "set grid\n");
   fprintf(gp, "set ylabel \"Error\"\n");
   fprintf(gp, "set xlabel \"#iteration\"\n");
   fprintf(gp, "p \'-\' w lp title \"bisection\"");
   fprintf(gp, ", \'-\' w lp title \"false-position\"");
   fprintf(gp, "\n");

   for(int i= 0; i< bsNum; ++i) {
      fprintf(gp, "%d %.6e\n", i, bsError[i]);
   }
   fprintf(gp, "e\n");
   for(int i= 0; i< fpNum; ++i) {
      fprintf(gp, "%d %.6e\n", i, fpError[i]);
   }
   fprintf(gp, "e\n");

   pclose(gp);
   gp= NULL;
}

int main(int argc, char *argv[])
{
   if(argc< 2) { return 1; }
   real precision= (real) (atof(argv[1])/ 100.0);

   real xl= 0.0, xu= 1.0;

   /* bisection */
   printf(" Bi-section:\n");
   int bsNum= (int) ceil(log((xu- xl)/ precision));
   bsNum= MIN(bsNum+1,100);
   real *bsError= bracketIteration(&bsNum, precision, xl, xu, &bisection);

   /* false position */
   printf(" False position:\n");
   int fpNum= 100;
   real *fpError= bracketIteration(&fpNum, precision, xl, xu, &falsePosition);

   gnuplotError(bsNum, bsError, fpNum, fpError);

   if(bsError) {
      free(bsError); bsError= NULL;
   }
   if(fpError) {
      free(fpError); fpError= NULL;
   }

   return 0;
}
