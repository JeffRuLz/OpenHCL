#pragma once

#include <math.h>
#include <stdlib.h>

#define PI 3.1415926535897932f

#define lengthdir_x(DIS,ANG) ( (DIS) * cos((ANG) * PI / 180) )
#define lengthdir_y(DIS,ANG) ( (DIS) * sin((ANG) * PI / 180) )

#define max(X,Y) ( ((X) > (Y))? (X): (Y) )
#define min(X,Y) ( ((X) < (Y))? (X): (Y) )

#define sign(X) ( ((X) > 0) - ((X) < 0) )

#define clamp(N,LO,HI) ( max((LO), min((N), (HI))) ) 

#define distance(X1,Y1,X2,Y2) ( sqrt(pow(X1-X2,2) + pow(Y1-Y2,2)) )
#define angle(X1,Y1,X2,Y2) (atan2((Y2) - (Y1), (X2) - (X1)) * 180 / PI)

#ifdef _NO_LERP
#define lerp(X,Y,Z) (X)
#else
#define lerp(X,Y,Z) ( ((X) == (Y))? (X): ((1.f - (Z)) * (Y) + (Z) * (X)) )
#endif