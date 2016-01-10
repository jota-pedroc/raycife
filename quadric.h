//  QUADRIC.H
//    Prototypes for QUADRIC.C

#ifndef _quadric_h
#define _quadric_h

typedef struct vec {// Vector structor
	double  x, y, z;// Coefficents of vector direction
}Vec;

typedef struct light {// Point light structure
	Vec    dir;// Direction to light
	double  Int;// Light intensity
}Light;

typedef struct mat {// Material structure
	double  r;// Red component
	double  b;// Blue component
	double  g;// Green component
	double  Ka;// Ambient reflection coefficent
	double  Kd;// Diffuse reflection coefficent
	double  Ks;// Specular reflection coefficent
	int  n;// Specular reflection exponent

}Mat;

typedef struct quad {// Quadric surface structure
	double  a, b, c, d, e;// Surface coefficents
	double  f, g, h, j, k;
	Mat     m;// Surface's material properties
}Quad;

typedef struct rayType {// Ray structure
	Vec  org;// Origin of ray
	Vec  dir;// Direction of ray
	int  depth;// Depth (or length) of ray
}Ray;


double  dot(Vec, Vec);
double  intersect(Ray ray, Quad *obj);
Vec     mkvec(double, double, double);
void    normalize(Vec*);
Vec     svmpy(double, Vec);
Vec     vadd(Vec, Vec);
Vec     vsub(Vec, Vec);

#endif

