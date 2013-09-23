#pragma once

#include "stdafx.h"

#define REAL double


REAL exactinit();
REAL orient2d(REAL *pa, REAL *pb, REAL *pc);
REAL orient3d(REAL *pa, REAL *pb, REAL *pc, REAL *pd);

/*               Return a positive value if the point pd lies inside the     */
/*               circle passing through pa, pb, and pc; a negative value if  */
/*               it lies outside; and zero if the four points are cocircular.*/
/*               The points pa, pb, and pc must be in counterclockwise       */
/*               order, or the sign of the result will be reversed.          */
REAL incircle(REAL *pa, REAL *pb, REAL *pc, REAL *pd);
