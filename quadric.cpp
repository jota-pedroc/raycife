#include "stdafx.h"
#include <math.h>
#include "quadric.h"


/*--------------------------------------------------------------------------*/
/*  QUADRIC.C    */
/*    This module contains the routine (and helper routines) to compute    */
/*    point of intersecion between a ray and a quadric surface    */
/*    */
/*- Modification History ---------------------------------------------------*/
/*  When:Who:Comments:    */
/*    */
/*  ??-???-??Victor KlassenInitial implementation    */
/*  03-Sep-95Christopher G. HealeyCombined code into a single module  */
/*--------------------------------------------------------------------------*/

double intersect(Ray ray, Quad *obj)

//  Compute the intersection point, if it exists, between the given ray
//  and the given object
//
//  Victor uses a strange formula from Watt & Watt for quadrics:
//
//     Ax^2 + Ey^2 + Hz^2 + Bxy + Fyz + Cxz + Dx + Gy + Jz + K
//
//  rather than the standard formula:
//
//     Ax^2 + By^2 + Cz^2 + 2Dxy + 2Eyz + 2Fxz + 2Gx + 2Hy + 2Jz + K
//
//  ray:  Ray being shot into scene
//  obj:  Object to test for intersection
{
	double  a, b, c, d, e;// Coefficents of equation of..
	double  f, g, h, j, k;// ..quadric surface
	double  acoef, bcoef, ccoef;// Intersection coefficents
	double  dx, dy, dz;// Direction - origin coordinates
	double  disc;// Distance to intersection
	double  root;// Root of distance to intersection
	double  t;// Distance along ray to intersection
	double  x0, y0, z0;// Origin coordinates


	a = obj->a;
	b = obj->b;
	c = obj->c;
	d = obj->d;
	e = obj->e;
	f = obj->f;
	g = obj->g;
	h = obj->h;
	j = obj->j;
	k = obj->k;

	dx = ray.dir.x;
	dy = ray.dir.y;
	dz = ray.dir.z;

	x0 = ray.org.x;
	y0 = ray.org.y;
	z0 = ray.org.z;

	acoef = 2 * f * dx * dz + 2 * e * dy * dz + c * dz * dz + b * dy * dy +
		a * dx * dx + 2 * d * dx * dy;

	bcoef = 2 * b * y0 * dy + 2 * a * x0 * dx + 2 * c * z0 * dz +
		2 * h * dy + 2 * g * dx + 2 * j * dz + 2 * d * x0 * dy +
		2 * e * y0 * dz + 2 * e * z0 * dy + 2 * d * y0 * dx +
		2 * f * x0 * dz + 2 * f * z0 * dx;

	ccoef = a * x0 * x0 + 2 * g * x0 + 2 * f * x0 * z0 + b * y0 * y0 +
		2 * e * y0 * z0 + 2 * d * x0 * y0 + c * z0 * z0 + 2 * h * y0 +
		2 * j * z0 + k;

	//  The following was modified by David J. Brandow to allow for planar
	//  quadrics

	if (1.0 + acoef == 1.0) {
		if (1.0 + bcoef == 1.0) {
			return -1.0;
		}

		t = (-ccoef) / (bcoef);

	}
	else {
		disc = bcoef * bcoef - 4 * acoef * ccoef;
		if (1.0 + disc < 1.0) {
			return -1.0;
		}

		root = sqrt(disc);
		t = (-bcoef - root) / (acoef + acoef);
		if (t < 0.0) {
			t = (-bcoef + root) / (acoef + acoef);
		}
	}

	if (t < 0.001)
		return -1.0;

	return t;
}// End procedure intersect
/*
*---------------------------------------------------
*---------------------------------------------------
*/
double dot(Vec v1, Vec v2)

//  This routine computes the dot product of two vectors
//
//  v1:  First vector
//  v2:  Second vector
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;

}

/*
*---------------------------------------------------
*---------------------------------------------------
*/
Vec mkvec(double x, double y, double z)

//  This routine combines the given coordinates into a Vec structure
//
//  x:  X-coordinate of direction of vector
//  y:  Y-coordinate of direction of vector
//  z:  Z-coordinate of direction of vector
{
	Vec  result;// Vector structure to hold result


	result.x = x;// Save coordinates
	result.y = y;
	result.z = z;
	return result;
}// End procedure mkvec

/*
*---------------------------------------------------
*---------------------------------------------------
*/
void normalize(Vec *v)

//  This routine normalizes the given vector
//
//  v:  Vec to normalize
{
	double denom;// Temporary denominator

	//  Absolute value of vector's coordinates

	double x = (v->x > 0.0) ? v->x : -v->x;
	double y = (v->y > 0.0) ? v->y : -v->y;
	double z = (v->z > 0.0) ? v->z : -v->z;


	if (x > y) {
		if (x > z) {
			y = y / x;
			z = z / x;
			denom = 1.0 / (x * sqrt(1.0 + y * y + z * z));

		}
		else { // z > x > y
			if (1.0 + z > 1.0) {
				y = y / z;
				x = x / z;
				denom = 1.0 / (z * sqrt(1.0 + y * y + x * x));
			}
		}

	}
	else {
		if (y > z) {
			z = z / y;
			x = x / y;
			denom = 1.0 / (y * sqrt(1.0 + z * z + x * x));

		}
		else { // x < y < z
			if (1.0 + z > 1.0) {
				y = y / z;
				x = x / z;
				denom = 1.0 / (z * sqrt(1.0 + y * y + x * x));
			}
		}
	}

	if (1.0 + x + y + z > 1.0) {
		*v = svmpy(denom, *v);
	}
}// End procedure normalize

/*
*---------------------------------------------------
*---------------------------------------------------
*/
Vec svmpy(double s, Vec v)

//  This routine multiples a vector by a constant
//
//  s:  Constant
//  v:  Vec
{
	Vec  result;// Vector structure to hold result


	result.x = s * v.x;
	result.y = s * v.y;
	result.z = s * v.z;
	return result;
}// End procedure svmpy

/*
*---------------------------------------------------
*---------------------------------------------------
*/
Vec vadd(Vec v1, Vec v2)

//  This routine adds two vectors
//
//  v1:  First vector
//  v2:  Second vector
{
	Vec  result;// Vector structure to hold result


	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;
	return result;
}// End procedure vadd

/*
*---------------------------------------------------
*---------------------------------------------------
*/
Vec vsub(Vec v1, Vec v2)

//  This routine subtracts two vectors
//
//  v1:  First vector
//  v2:  Second vector
{
	Vec  result;// Vector structure to hold result


	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;
	return result;
}// End procedure vsub

