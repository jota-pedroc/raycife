// ProjetoCG.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ProjetoCG.h"
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <thread>
#include <algorithm> 
#include <random> 


#include "Cena.h"
#include "Objeto.h"
#include "Raio.h"
#include "omp.h"

using namespace std;

#define N_THREADS 3
#define MAX_RECURSION_DEPTH 5

Buffer buf;
//Recebe 800x600 como defalt mas é alterada dependendo do arquivo de entrada
static GLfloat window_width = 800.0;
static GLfloat window_height = 600.0;

//Loading camera paramenters
Olho olho;

//Loading view paramenters
Janela janela;

//Loading scene paramenters
Cena cena;

//Loading illumination paramenters
Luz luz;

Color** texture;
int textureX = 200;
int textureY = 200;

//Loading objects of the scene
vector<Objeto> objetos;
vector<Quadrica> quadricas;

class Intersection {
public:
	Objeto objeto;
	Ponto p;
	Vetor normal;
	bool hit;
private:
	Face f;
};

Intersection bInt1[300][300];
Ponto testPoints[500];
int countPoints = 0;

//Color texture[50][50];

///-------------------------OTHER PEOPLE'S STUFF-----------------------------------------///
//http://www.lighthouse3d.com/tutorials/maths/ray-triangle-intersection/
//http://www.cs.virginia.edu/~gfx/Courses/2003/ImageSynthesis/papers/Acceleration/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf

//// may be worth checking this one too: http://geomalgorithms.com/a06-_intersect-2.html
///* a = b - c */
#define vector(a,b,c) \
	(a)[0] = (b)[0] - (c)[0];	\
	(a)[1] = (b)[1] - (c)[1];	\
	(a)[2] = (b)[2] - (c)[2];

float innerProduct(float* v1, float* v2){
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

//A x B = (a2b3 - a3b2, a3b1 - a1b3, a1b2 - a2b1); a vector quantity
void crossProduct(float* result, float* v1, float* v2){
	result[0] = v1[1] * v2[2] - v1[2] * v2[1];
	result[1] = v1[2] * v2[0] - v1[0] * v2[2];
	result[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

float rayIntersectsTriangle(float *p, float *d,
	float *v0, float *v1, float *v2) {

	float e1[3], e2[3], h[3], s[3], q[3];
	float a, f, u, v, t;
	vector(e1, v1, v0);
	vector(e2, v2, v0);

	crossProduct(h, d, e2);
	a = innerProduct(e1, h);

	if (a > -0.00001 && a < 0.00001)
		return(-1);

	f = 1 / a;
	vector(s, p, v0);
	u = f * (innerProduct(s, h));

	if (u < 0.0 || u > 1.0)
		return(-1);

	crossProduct(q, s, e1);
	v = f * innerProduct(d, q);

	if (v < 0.0 || u + v > 1.0)
		return(-1);

	// at this stage we can compute t to find out where
	// the intersection point is on the line
	t = f * innerProduct(e2, q);

	if (t > 0.00001) // ray intersection
		return(t);
	
	else // this means that there is a line intersection
		// but not a ray intersection
		return (-1);

}
///------------------------- END OF OTHER PEOPLE'S STUFF-----------------------------------------///



bool retorno = false;
double dist = INT_MAX;
double distTemp = -1;


Vetor calcularRefracao(float n1, float n2, Vetor i, Vetor n){
	float cosI = -escalar(i, n);

	float sen2t = pow(n1 / n2, 2)*(1 - pow(cosI, 2));

	Vetor t = vsum(kprod(n1 / n2, i),kprod(((n1 / n2)*cosI - sqrt(1 - sen2t)), n));
	return t;
}


Color difuso(Color ip, float kd, Vetor lightDir, Vetor normal, Color corObjeto){

	Color retorno;
	float prodEscalar = escalar(lightDir, normal);		
	float aux = kd*prodEscalar;
	retorno.r = ip.r*aux*corObjeto.r; 
	retorno.g = ip.g*aux*corObjeto.g;
	retorno.b = ip.b*aux*corObjeto.b;
	
	return retorno;
};


float rand01(){
	random_device rd;
	return ((float)rd()/rd.max());
}



Vetor normalEsfera(Ponto p, Quadrica q){
	Ponto centro;
	centro.x = -q.g;
	centro.y = -q.h;
	centro.z = -q.j;
	return defVetor(centro, p);
}

Intersection closestObject(Raio ray, Cena scene){
	// TODO
	Intersection out;
	Objeto current;
	vector<Face> currentFaces;
	Face face;
	float closestDist = INT_MAX;

	for (int i = 0; i < objetos.size(); i++)
	{
		current = objetos.at(i);
		
		currentFaces = current.faces;
		for (int j = 0; j < currentFaces.size(); j++)
		{
			Face f = currentFaces.at(j);
			Vertice* x = f.v1;
			float v0[3] = { x->x, x->y, x->z };
			Vertice* y = f.v2;
			float v1[3] = { y->x, y->y, y->z };
			Vertice* z = f.v3;
			float v2[3] = { z->x, z->y, z->z };

			float p[3] = { ray.posicao.x, ray.posicao.y, ray.posicao.z };
			float d[3] = { ray.direcao.x, ray.direcao.y, ray.direcao.z };
			float t = rayIntersectsTriangle(p,d,v0,v1,v2);
			if (t > 0 && t < closestDist){
					closestDist = t;
					out.objeto = current;
					out.p.x = ray.direcao.x*t + ray.posicao.x;
					out.p.y = ray.direcao.y*t + ray.posicao.y;
					out.p.z = ray.direcao.z*t + ray.posicao.z;
					out.normal = current.normalPonto(out.p, f);
			}
		}
	}

	Quadrica currentQuad;
	for (int i = 0; i < quadricas.size(); i++)
	{
		currentQuad = quadricas.at(i);
		
		float t = intersect(ray.toRay(), &(currentQuad.toQuad()));
		if (t > 0 && t < closestDist){
			closestDist = t;
			out.objeto = currentQuad;
			out.p.x = ray.direcao.x*t + ray.posicao.x;
			out.p.y = ray.direcao.y*t + ray.posicao.y;
			out.p.z = ray.direcao.z*t + ray.posicao.z;;
			out.normal = normalEsfera(out.p, currentQuad);
		}
		
		
	}

	if (closestDist != INT_MAX){
		out.hit = true;
	}
	else {
		out.hit = false;
	}

	return out;
}

Raio cameraRay(int x, int y, Janela jan, Olho o){
	/*
	* Trace a ray from the camera to the pixel (x,y) on the window
	*/

	Raio output; // Output variable
	float xw, yw, zw, sizexw, sizeyw; // Pixel location and window size on world coordinates
	Vetor direcao, posicao; // Ray's direction and position

	sizexw = jan.x1 - jan.x0;
	sizeyw = jan.y0 - jan.y1;
	xw = ((float)x / jan.sizeX)*sizexw + jan.x0;
	yw = ((float)y / jan.sizeY)*sizeyw + jan.y1;
	zw = 0;

	posicao.x = o.x;
	posicao.y = o.y;
	posicao.z = o.z;

	direcao.x = xw - o.x;
	direcao.y = yw - o.y;
	direcao.z = zw - o.z;

	output.posicao = posicao;
	output.direcao = direcao;
	output.direcao = normalizar(output.direcao);
	return output;
}

// --------------------------SHADOW TEXTURE--------------------------

Intersection intersectTexture(Objeto  texture, Raio ray, Cena scene){
	Intersection out;
	Objeto current;
	vector<Face> currentFaces;
	Face face;
	float closestDist = INT_MAX;

	current = texture;
	currentFaces = current.faces;
	for (int j = 0; j < currentFaces.size(); j++)
	{
		Face f = currentFaces.at(j);
		Vertice* x = f.v1;
		float v0[3] = { x->x, x->y, x->z };
		Vertice* y = f.v2;
		float v1[3] = { y->x, y->y, y->z };
		Vertice* z = f.v3;
		float v2[3] = { z->x, z->y, z->z };

		float p[3] = { ray.posicao.x, ray.posicao.y, ray.posicao.z };
		float d[3] = { ray.direcao.x, ray.direcao.y, ray.direcao.z };
		float t = rayIntersectsTriangle(p, d, v0, v1, v2);
		if (t > 0 && t < closestDist){
			closestDist = t;
			out.objeto = current;
			out.p.x = ray.direcao.x*t + ray.posicao.x;
			out.p.y = ray.direcao.y*t + ray.posicao.y;
			out.p.z = ray.direcao.z*t + ray.posicao.z;
			out.normal = current.normalPonto(out.p, f);
		}
	}
	if (closestDist != INT_MAX){
		out.hit = true;
		return out;
	}
	else {
		out.hit = false;
		return out;
	}
}

Raio cameraTexture(int x, int y, int bufferSizeX, int bufferSizeY, Luz l){
	/*
	* Trace a ray from the camera to the pixel (x,y) on the window
	*/

	Raio output; // Output variable
	float xw, yw, zw, sizexw, sizezw; // Pixel location and window size on world coordinates
	Vetor direcao, posicao; // Ray's direction and position

	float x0 = -3.822;
	float y0 = -32.76;
	float x1 = 3.822;
	float y1 = -16.59;

	sizexw = x1 - x0;
	sizezw = y1 - y0;
	xw = ((float)x / bufferSizeX)*sizexw + x0;
	zw = ((float)y / bufferSizeY)*sizezw + y0;
	yw = 2.0f;


	posicao.x = l.ponto.x;
	posicao.y = l.ponto.y;
	posicao.z = l.ponto.z;

	direcao.x = xw - l.ponto.x;
	direcao.y = yw - l.ponto.y;
	direcao.z = zw - l.ponto.z;

	output.posicao = posicao;
	output.direcao = direcao;
	output.direcao = normalizar(output.direcao);
	return output;
}


Color** createTexture(Objeto objectTexture){
	int xsize = textureX; // width in pixels
	int ysize = textureY; // height in pixels

	Color** img; // output img
	Raio ray; // Camera to window variable, used for each different pixel

	img = (Color**)malloc(sizeof(Color*)*xsize); // Array instanciation	

	//-----------------------------------------------MAIN LOOP----------------------------------------------

	for (int i = 0; i < xsize; i++)
	{
		img[i] = (Color*)malloc(sizeof(Color)*ysize); // Array instanciation
		for (int j = 0; j < ysize; j++)
		{
			ray = cameraTexture(i, j, xsize, ysize, cena.luz);

			Intersection intersect = intersectTexture(objectTexture, ray, cena);

			if (intersect.hit){
				Color out = Color(0, 0, 0);
				img[i][j] = out;
			}
			else{
				Color out = Color(255, 255, 255);
				img[i][j] = out;
			}
		}
	}

	return img;
}

//Mapping texture
Color** applyTexture(Objeto objectTexture, Color** buffer){
	int xsize = 200; // width in pixels
	int ysize = 200; // height in pixels

	Color** img; // output img
	Raio ray; // Camera to window variable, used for each different pixel

	img = (Color**)malloc(sizeof(Color*)*xsize); // Array instanciation	

	//-----------------------------------------------MAIN LOOP----------------------------------------------

	for (int i = 0; i < xsize; i++)
	{
		img[i] = (Color*)malloc(sizeof(Color)*ysize); // Array instanciation
		for (int j = 0; j < ysize; j++)
		{
			ray = cameraRay(i, j, janela, olho);

			Intersection intersect = closestObject(ray, cena);

			if (intersect.hit){//Tests if the ray hits any object at the scene
				Ponto hitPoint = intersect.p;

				//Cast another ray in the light direction in order to get the texture
				Vetor newVector = defVetor(hitPoint, cena.luz.ponto);
				newVector = normalizar(newVector); //normalizing it
				Raio newRay;
				newRay.direcao = newVector;
				newRay.posicao.x = hitPoint.x;
				newRay.posicao.y = hitPoint.y;
				newRay.posicao.z = hitPoint.z;
				Intersection planeIntersect = intersectTexture(objectTexture, newRay, cena);

				if (planeIntersect.hit){//If hit the plane continue
					float x0 = -3.822;
					float x1 = 3.822;
					float z0 = -32.76;
					float z1 = -16.59;

					Ponto p = planeIntersect.p;

					int mapX = floor(((p.x - x0)*textureX) / (x1 - x0));
					int mapY = floor(((p.z - z0)*textureY) / (z1 - z0));

					//Making sure that everything stays inside the array limit
					if (mapX < 0) mapX = 0;
					if (mapY < 0) mapY = 0;
					if (mapX >= textureX) mapX = textureX - 1;
					if (mapX >= textureY) mapY = textureY - 1;


					if (texture[mapX][mapY].r == 0){//Verifies on the texture if it is a shadow point
						buffer[i][j] = Color(0, 0, 0);
					}
				}
			}
		}
	}

	return img;
}


bool shadowRay(Raio ray, Cena scene){
	bool retorno = false;
	Intersection intersection = closestObject(ray, scene);
	Objeto closest = intersection.objeto;
	if (closest.isLight) return false;
	
	if (intersection.hit) retorno = true;
	return retorno;
}

Vetor rotacionar(float angle, Vetor v, Vetor eixo){
	float matrix[3][3];
	float co = cos(angle);
	float si = sin(angle);
	float a = eixo.x;
	float b = eixo.y;
	float c = eixo.z;
	matrix[0][0] = co + (1 - co)*pow(a, 2);
	matrix[0][1] = (1 - co)*a*b + si*c;
	matrix[0][2] = (1 - co)*a*c - si*b;
	matrix[1][0] = (1 - co)*b*a - si*c;
	matrix[1][1] = co + (1 - co)*pow(b, 2);
	matrix[1][2] = (1 - co)*b*c + si*a;
	matrix[2][0] = (1 - co)*a*c + si*b;
	matrix[2][1] = (1 - co)*b*c - si*a;
	matrix[2][2] = co + (1 - co)*pow(b, 2);
	Vetor out;
	out.x = matrix[0][0] * v.x + matrix[0][1] * v.y + matrix[0][2] * v.z;
	out.y = matrix[1][0] * v.x + matrix[1][1] * v.y + matrix[1][2] * v.z;
	out.z = matrix[2][0] * v.x + matrix[2][1] * v.y + matrix[2][2] * v.z;
	return out;
}

Color trace_path(int depth, Raio ray, Cena scene, Luz luz, int i, int j, int nSample){
	if (depth >= MAX_RECURSION_DEPTH) return Color(0,0,0);

	//cout << depth << endl;

	Color output;

	// --------------------------check intersections--------------------------
	// ray intersects triangle for each triangle. Get the one closest to the eye that returns true.
	Intersection intersection;

		if (depth == 0 && nSample == 0){
			intersection = closestObject(ray, scene);
			bInt1[i][j] = intersection;
		}
		else if (depth == 0 && nSample != 0){
			intersection = bInt1[i][j];
		}
		else{
			intersection = closestObject(ray, scene);
		}


		if (intersection.hit == false){
			return scene.background;
		}
		else if (intersection.objeto.isLight){
			return luz.cor;
		}
	
	
	Objeto closest = intersection.objeto;
	Ponto inters = intersection.p;
	Vetor normal = intersection.normal;
	normal = normalizar(normal);

	float bias = 1e-4;//Use in order to avoid noise

	//Sortear um ponto de luz aleatório dentro do obj Luz
	int triangulo = rand() % 2;
	Face triLuz = objetos.at(0).faces.at(triangulo);
	double alpha = rand() %100;
	double beta = rand() % 100;
	double gama = rand() % 100;
	double sum = alpha + beta + gama;
	alpha = alpha / sum;
	beta = beta / sum;
	gama = gama / sum;

	Vertice* v1 = triLuz.v1;
	Vertice* v2 = triLuz.v2;
	Vertice* v3 = triLuz.v3;

	Ponto lightRand;

	lightRand.x = alpha*v1->x + beta*v2->x+gama*v3->x;
	lightRand.y = v1->y;
	lightRand.z = alpha*v1->z + beta*v2->z + gama*v3->z;

	//Opçao sem ruido menos automático
	/*double a = (rand() %100)/100;
	double b = (rand() % 100)/100;
	double distX = 0.88 - (-0.88);
	double distY = -23.35 - (-26.45);

	distX = distX*a;
	distY = distY*b;

	Ponto lightRand;
	lightRand.x = -0.88+distX;
	lightRand.y = 3.83;
	lightRand.z = -26.45 + distY;*/

	Vetor toLight = defVetor(inters, cena.luz.ponto);
	toLight = normalizar(toLight);
	float kd = closest.kd, ks = closest.ks, kt = closest.kt;
	

	// ---------------------------color calculation----------------------------
	//Rambiente = Ia*kar
	float iA = scene.ambiente;
	Vetor ambiente = kprod(iA*closest.ka, closest.cor.toVetor());

	//Rdifuso = Ip*kd(L.N)r
	Color difusa = difuso(luz.cor, closest.kd, toLight, normal, closest.cor);

	//Respecular = Ip*ks*(R.V)^n
	Vetor rVetor =subVetor(kprod(2 * escalar(toLight,normal), normal), toLight);
	rVetor = normalizar(rVetor);
	Vetor vVetor = kprod(-1, ray.direcao);
	vVetor = normalizar(vVetor);
	Color especular;
	float aux = pow(escalar(rVetor, vVetor), closest.coeficienteEspecular);
	aux = luz.Ip*closest.ks*aux;
	especular.r = luz.cor.r*aux;
	especular.g = luz.cor.g*aux;
	especular.b = luz.cor.b*aux;


	//Shadow Ray
	Raio ray2;
	ray2.direcao =  defVetor(inters, lightRand);

	//Walk a little bit in the normal direction in order to avoid self intersection
	Vetor dist = kprod(bias, normal);
	
	ray2.posicao.x = inters.x + dist.x;
	ray2.posicao.y = inters.y + dist.y;
	ray2.posicao.z = inters.z + dist.z;

	bool sombra = shadowRay(ray2,scene);

	////Definindo o valor da cor local
	Color corLocal;
	if (sombra){
		corLocal.r = 0;
		corLocal.g = 0;
		corLocal.b = 0;
	}else{
		corLocal = csum(csum(difusa, Color(ambiente)), especular);
	}
	
	//Ray cast from the recursion
	Raio novoRaio;
	novoRaio.posicao.x = inters.x;
	novoRaio.posicao.y = inters.y;
	novoRaio.posicao.z = inters.z;

	// -------------------------recursion for contribution from other objects---------------------------------
	float ktot = kd + ks + kt;
	float r = rand01()*ktot;
	Vetor direcao = Vetor(), posicao = Vetor();
	if (r < kd){
		// raio difuso
		float  r1 = 2 * PI * rand01();  // random angle around
		float r2 = rand01();           // random distance from center
		float r2s = sqrt(r2);          // square root of distance from center

		Vetor w = normal;           // set first axis equal to normal
		Vetor v1;
		v1.x = 0;
		v1.y = 1;
		v1.z = 0;
		Vetor v2;
		v2.x = 1;
		v2.y = 0;
		v2.z = 0;
		Vetor u = fabs(w.x) > 0.1 ? v1 : v2;
		u = (normalizar(vetorial(u, w)));      // second axis
		Vetor v = vetorial(w, u);          // final axis

		// random direction 
		Vetor psi;
		psi.x = u.x*cos(r1)*r2s + v.x*sin(r1)*r2s + w.x*sqrt(1 - r2);
		psi.y = u.y*cos(r1)*r2s + v.y*sin(r1)*r2s + w.y*sqrt(1 - r2);
		psi.z = u.z*cos(r1)*r2s + v.z*sin(r1)*r2s + w.z*sqrt(1 - r2);

		psi = normalizar(psi);

		direcao = psi;
	}
	else if (r < kd + ks){
		// raio especular
		// direcao: R=2N(NL) - L
		direcao = subVetor(kprod(2 * escalar(normal, toLight), normal), toLight);

	}
	else {
		// raio transmitido
		// TODO
		// objeto opaco? nenhuma cor transmitida
		// caso contrario... verificar refracao


		float cos = escalar(ray.direcao, normal);
		float n1, n2;
		if (cos > 0){
			//Inside the object
			n1 = closest.coeficienteRefracao;
			n2 = 1;
			direcao = calcularRefracao(n1, n2, ray.direcao, kprod(-1, normal));
			normal = kprod(-1, normal);
		}
		else {
			n1 = 1;
			n2 = closest.coeficienteRefracao;
			direcao = calcularRefracao(n1, n2, ray.direcao, normal);
		}

		Vetor dist2 = kprod(bias, normal);
		novoRaio.posicao.x = inters.x - dist2.x;
		novoRaio.posicao.y = inters.y - dist2.y;
		novoRaio.posicao.z = inters.z - dist2.y;
	}

	// Use the direction and position vector to make a ray

	novoRaio.direcao = direcao;
	novoRaio.direcao = normalizar(novoRaio.direcao);
	float cos_theta = escalar(direcao, normal);

	Color emitance;
	float auxEmtR = escalar(normal, toLight)*cena.luz.Ip*closest.cor.r;
	float auxEmtG = escalar(normal, toLight)*cena.luz.Ip*closest.cor.g;
	float auxEmtB = escalar(normal, toLight)*cena.luz.Ip*closest.cor.b;
	emitance = Color(auxEmtR, auxEmtG, auxEmtB);
	

	Color BRDF;
	BRDF.r = 2 * emitance.r * cos_theta;
	BRDF.g = 2 * emitance.g * cos_theta;
	BRDF.b = 2 * emitance.b * cos_theta;

	Color recursion = trace_path(depth + 1, novoRaio, scene, scene.luz, i, j, nSample);
	
	// -----------------------------------output--------------------------------------
	//*****Testar diferentes pesos*****

	/*output.r = (recursion.r  + corLocal.r)*kd;
	output.g = (recursion.g + corLocal.g)*kd;
	output.b = (recursion.b + corLocal.b)*kd;*/
	/*
	output.r = recursion.r*.5 + corLocal.r*.5;
	output.g = recursion.g*.5 + corLocal.g*.5;
	output.b = recursion.b *.5 + corLocal.b*.5;
	*/

	float factor = max(max(kd, ks), kt);

	output.r = (recursion.r + corLocal.r)*factor;
	output.g = (recursion.g + corLocal.g)*factor;
	output.b = (recursion.b + corLocal.b)*factor; 

	return output;
}


void testingDifuse(){
	
	Intersection intTest = bInt1[130][165];
	Vetor normal = intTest.normal;
	normal = normalizar(normal);
	
	for (int i = 0; i < 500; i++)
	{
		float  r1 = 2 * PI * rand01();  // random angle around
		float r2 = rand01();           // random distance from center
		float r2s = sqrt(r2);          // square root of distance from center
		Vetor direcao;
		Vetor w = normal;           // set first axis equal to normal
		Vetor v1;
		v1.x = 0;
		v1.y = 1;
		v1.z = 0;
		Vetor v2;
		v2.x = 1;
		v2.y = 0;
		v2.z = 0;
		Vetor u = fabs(w.x) > 0.1 ? v1 : v2;
		u = (normalizar(vetorial(u, w)));      // second axis
		Vetor v = vetorial(w, u);          // final axis

		// random direction 
		Vetor psi;
		psi.x = u.x*cos(r1)*r2s + v.x*sin(r1)*r2s + w.x*sqrt(1 - r2);
		psi.y = u.y*cos(r1)*r2s + v.y*sin(r1)*r2s + w.y*sqrt(1 - r2);
		psi.z = u.z*cos(r1)*r2s + v.z*sin(r1)*r2s + w.z*sqrt(1 - r2);

		psi = normalizar(psi);

		direcao = psi;

		Raio newRay;
		newRay.direcao = direcao;
		newRay.posicao.x = intTest.p.x;
		newRay.posicao.y = intTest.p.y;
		newRay.posicao.z = intTest.p.z;

		Intersection nextPoint = closestObject(newRay, cena);

		if (nextPoint.hit){
			testPoints[countPoints]=nextPoint.p;
			countPoints++;
		}
	}

	//Rendering points
	for (int i = 0; i < 200; i++)
	{
		for (int j = 0; j < 200; j++)
		{
			
			for (int k = 0; k < countPoints; k++)
			{
				float diffX =fabs( testPoints[k].x - bInt1[i][j].p.x);
				float diffY = fabs(testPoints[k].y - bInt1[i][j].p.y);
				float diffZ = fabs(testPoints[k].z - bInt1[i][j].p.z);
				if (diffX<0.1 && diffY<0.1 && diffZ<0.1)
				buf.buffer[i][j] = Color(255,0,255);
			}
		}
	}
}

float clamp(float x){ return x<0 ? 0 : x>1 ? 1 : x; };

float tColor(float x){ return pow(clamp(x), 1 / 2.2); };

void renderThread(int id, Color** output, int x, int xmax, int y, int ymax, Janela jan, Cena scene, Olho o, Luz luz){
	/*
	* Render a image using the path tracing algorithm for the given scene, camera and window.
	*/

	int xsize = xmax - x; // width in pixels
	int ysize = ymax - y; // height in pixels

	int nSamples = scene.npaths; // number of color samples per pixel

	float count = 0, maxCount = xsize*ysize*nSamples, blockSize = maxCount / 10, blockCount = blockSize;
	Color** img = output; // output img
	Color sum, sample; // Acumulator and sample variables, used for each different pixel and pixel sample, respectively
	Raio ray; // Camera to window variable, used for each different pixel


	//-----------------------------------------------MAIN LOOP----------------------------------------------
	// For each pixel, take nSample of colors and average them. The average is the color of that pixel.
	time_t startTime;
	time(&startTime);
	for (int i = x; i < xmax; i++)
	{
		for (int j = y; j < ymax; j++)
		{
			sum.r = 0;
			sum.g = 0;
			sum.b = 0;
			ray = cameraRay(i, j, jan, o);
			if (i > 100 && i < 110 && j > 180 && j < 190){
				int debug = 2 + 2;
			}
			for (int k = 0; k < nSamples; k++)
			{

				sample = trace_path(0, ray, scene, luz,i,j,k);
				sum = csum(sum, sample);
				count++;
				if (count > blockCount){
					time_t elapsed = time(nullptr) - startTime;
					//time_t remaining = (elapsed *(1 - (count / maxCount))) / (count / maxCount);
					printf("Thread %d: Processing... %d%% (%d/%d). Elapsed time: %ds.\n", id, (int)((count / maxCount) * 100), (int)count, (int)maxCount, elapsed);
					blockCount += blockSize;
				}

			}


			Color out = Color(sum.r / nSamples, sum.g / nSamples, sum.b / nSamples);

			//Applying Tone Mapping 
			//out = Color(out.r / (out.r + cena.tonemapping), out.g / (out.g + cena.tonemapping), out.b / (out.b + cena.tonemapping));

			Color newOut = Color(out.r, out.g, out.b);
			img[i][j] = newOut;

			//img[i][j] = Color(sum.r/nSamples, sum.g/nSamples, sum.b/nSamples);
		}
	}
}

Color** render(Janela jan, Cena scene, Olho o, Luz luz){
	/*
	* Render a image using the path tracing algorithm for the given scene, camera and window.
	*/

	int xsize = jan.sizeX; // width in pixels
	int ysize = jan.sizeY; // height in pixels

	int nSamples = scene.npaths; // number of color samples per pixel

	float count = 0, maxCount = xsize*ysize*nSamples, blockSize = maxCount / 100, blockCount = blockSize;
	Color** img; // output img
	 // Acumulator and sample variables, used for each different pixel and pixel sample, respectively
	Raio ray; // Camera to window variable, used for each different pixel

	img = (Color**)malloc(sizeof(Color*)*xsize); // Array instanciation	

	//-----------------------------------------------MAIN LOOP----------------------------------------------
	// For each pixel, take nSample of colors and average them. The average is the color of that pixel.
	time_t startTime;
	time(&startTime);
	
	for (int i = 0; i < xsize; i++)
	{
		img[i] = (Color*)malloc(sizeof(Color)*ysize); // Array instanciation
	}

	#pragma omp parallel for
	for (int i = 0; i < xsize; i++)
	{
		
		for (int j = 0; j < ysize; j++)
		{
			Color sum, sample;

			sum.r = 0;
			sum.g = 0;
			sum.b = 0;
			ray = cameraRay(i, j, jan, o);
			/*if (i > 100 && i < 110 && j > 180 && j < 190){
				int debug = 2 + 2;
				}*/
			for (int k = 0; k < nSamples; k++)
			{

				sample = trace_path(0, ray, scene, luz,i,j,k);
				sum = csum(sum, sample);
				//count++;
				//if (count > blockCount){
				//	time_t elapsed = time(nullptr) - startTime;
				//	//time_t remaining = (elapsed *(1 - (count / maxCount))) / (count / maxCount);
				//	printf("Processing... %d%% (%d/%d). Elapsed time: %ds.\n", (int)((count / maxCount) * 100), (int)count, (int)maxCount, elapsed);
				//	blockCount += blockSize;
				//}

			}


			Color out = Color(sum.r / nSamples, sum.g / nSamples, sum.b / nSamples);

			//Applying Tone Mapping 
			Color newOut = Color(out.r / (out.r + cena.tonemapping), out.g / (out.g + cena.tonemapping), out.b / (out.b + cena.tonemapping));
			img[i][j] = newOut;

			//img[i][j] = Color(sum.r/nSamples, sum.g/nSamples, sum.b/nSamples);
		}
		cout << "done " << i << endl;
	}
	
	return img;
}


void myinit()
{
	//srand(time(NULL));
	//    estado = MODIFIED;

	//Imagem projetada no near plane sera desenhada na seguinte area da tela: (0,0,width,height)
	glLoadIdentity();
	gluOrtho2D(-1, 1, -1, 1);
	glViewport(0, 0, window_width, window_height);

	//Setando tipo de projecao..
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//Projecao sera ortogonal
	//glOrtho(0, window_width, window_height, 0, 0, 5.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	// loop(0);
}




void renderScene()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0, 0, 0);

	//pixel width
	for (int i = 0; i < window_width; i++)
	{
		//pixel height
		for (int j = 0; j < window_height; j++)
		{
				glBegin(GL_POINTS);
				glColor3f(buf.buffer[i][j].r, buf.buffer[i][j].g, buf.buffer[i][j].b); 
				glVertex2d((i) / (window_width / 2) - 1, 1 - (j) / (window_height / 2));
				glEnd();
		}
	}

	glFlush();
}

int main(int argc, char **argv)
{
	//Lendo arquivo sdl que descreve a cena utilizada e calculando a normal após
	lerCena("cornel_box\\cornellroom.sdl",cena);

	//Loading camera paramenters
	olho = cena.olho;

	//Loading view paramenters
	janela = cena.janela;
	
	cena.luz.ponto.x = 0;
	cena.luz.ponto.y = 3.8360;
	cena.luz.ponto.z = -24; //----------->light see how to load the file
		  
	//Loading objects of the scene
	objetos = cena.objetos;
	quadricas = cena.quadricas;

	
	window_height = janela.sizeY;
	window_width = janela.sizeX;


	//Carregando os objetos no vector de objetos
	for (int i = 0; i < objetos.size(); i++)
	{
		char realPath [100]= "cornel_box\\";
		strcat(realPath, objetos.at(i).path);
		lerObjeto(realPath, objetos.at(i));
		objetos.at(i).normalVertice();
	}

	//////Chamando o algoritmos de renderização que inclui o Path Tracing
	Color** img = (Color**) malloc(sizeof(Color*)*janela.sizeX);
	for (int i = 0; i < janela.sizeX; i++)
	{
		img[i] = (Color*) malloc(sizeof(Color)*janela.sizeY);
	}

	thread threads[N_THREADS];
	//int blockX = janela.sizeX / N_THREADS;
	int blockX = janela.sizeX;
	int blockY = janela.sizeY / N_THREADS;
	int x = 0, y = 0, xmax = blockX, ymax = blockY;
	for (int i = 0; i < N_THREADS; i++)
	{
		threads[i] = thread(renderThread, i, img, x, xmax, y, ymax, janela, cena, cena.olho, cena.luz);
		//x += blockX;
		//xmax += blockX;
		y += blockY;
		ymax += blockY;
	}
	for (int i = 0; i < N_THREADS; i++)
	{
		threads[i].join();
	}
	buf.buffer = img;
	//testingDifuse();

	//buf.buffer = render(janela,cena,olho,luz);

	///////////////////////////////////////////Texture//////////////////////////////////////////////

	//////Creating texture and storing into a array
	//texture = createTexture(objetos.at(7));

	//////Carregando o objeto que representa o plano da textura
	//Objeto textureObject;
	//char realPath[100] = "cornel_box\\";
	//strcat(realPath, "texture.obj");
	//lerObjeto(realPath, textureObject);

	//applyTexture(textureObject, buf.buffer);
	
	
	///////////////////////////////////////////OPENGL//////////////////////////////////////////////
	//Initiating glut variables
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(window_width, window_height); //colocar parametros de entrada
	glutInitWindowPosition(0, 100);//optional
	glutCreateWindow("OpenGL First Window");


	// register callbacks
	glutDisplayFunc(renderScene);

	myinit();

	glutMainLoop();
	
	return 0;
}

