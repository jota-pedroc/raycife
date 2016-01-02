// ProjetoCG.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ProjetoCG.h"
#include <iostream>
#include <fstream>
#include <string>



///-------------------------OTHER PEOPLE'S STUFF-----------------------------------------///
//http://www.lighthouse3d.com/tutorials/maths/ray-triangle-intersection/
//http://www.cs.virginia.edu/~gfx/Courses/2003/ImageSynthesis/papers/Acceleration/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf

// may be worth checking this one too: http://geomalgorithms.com/a06-_intersect-2.html
/* a = b - c */
#define vector(a,b,c) \
	(a)[0] = (b)[0] - (c)[0];	\
	(a)[1] = (b)[1] - (c)[1];	\
	(a)[2] = (b)[2] - (c)[2];

int rayIntersectsTriangle(float *p, float *d,
	float *v0, float *v1, float *v2) {

	float e1[3], e2[3], h[3], s[3], q[3];
	float a, f, u, v;
	vector(e1, v1, v0);
	vector(e2, v2, v0);

	//crossProduct(h, d, e2);
	//a = innerProduct(e1, h);

	if (a > -0.00001 && a < 0.00001)
		return(false);

	f = 1 / a;
	vector(s, p, v0);
	//u = f * (innerProduct(s, h));

	if (u < 0.0 || u > 1.0)
		return(false);

	//crossProduct(q, s, e1);
	//v = f * innerProduct(d, q);

	if (v < 0.0 || u + v > 1.0)
		return(false);

	// at this stage we can compute t to find out where
	// the intersection point is on the line
	//t = f * innerProduct(e2, q);

	//if (t > 0.00001) // ray intersection
		return(true);

	//else // this means that there is a line intersection
		// but not a ray intersection
		return (false);

}
///------------------------- END OF OTHER PEOPLE'S STUFF-----------------------------------------///

Objeto closestObject(Raio ray, Cena scene){
	// TODO
}

Vetor Color::toVetor(){
	Vetor out;
	out.x = this->r;
	out.y = this->g;
	out.z = this->b;
}



Color difuso(float ip, float kd, Vetor lightDir, Vetor normal);

void renderScene(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0, 0.0, 0.0, 1.0);//clear red

	glutSwapBuffers();
}

Vetor mkvec(Ponto p, Ponto q){
	Vetor out;
	out.x = p.x - q.x;
	out.y = p.y - q.y;
	out.z = p.z - q.z;
	return out;
}

float rand01(){
	return ((float)rand() / (RAND_MAX));
}

Color trace_path(int depth, Raio ray, Cena scene){
	if (depth == 5) return scene.background;

	Color output;

	// --------------------------check intersections--------------------------
	// ray intersects triangle for each triangle. Get the one closest to the eye that returns true.
	Objeto closest; // TODO
	Vetor normal, toLight;
	float kd = closest.kd, ks = closest.ks, kt = closest.kt;
	Ponto inters;
	// ---------------------------color calculation----------------------------
	float iA;
	// TODO: Ia = scene->ambiente
	Vetor ambiente = kprod(iA*closest.ka, closest.cor.toVetor());
	Luz l;
	// TODO:l = scene.luz
	int lintensidade; Ponto normal;// TODO : temp

	Color difusa = difuso(lintensidade, closest.kd, mkvec(l.ponto, inters), normal);


	// -------------------------recursion for contribution from other objects---------------------------------
	float ktot = kd + ks + kt;
	float r = rand01()*ktot;
	Vetor direcao, posicao;
	if (r < kd){
		// raio difuso
		//phi=cos-1 (sqrt(R1)) e theta = 2.pi.R2.
		float r2 = rand01();
		float theta = 2 * PI * r2;
		float r1 = rand01();
		float phi = acos(sqrt(r1));

		// build the direction vector with phi and theta
		// Normalized v!!
		// x = cos phi
		// y = sen phi
		// z = sen theta
		direcao.x = cos(phi);
		direcao.y = sin(phi);
		direcao.z = sin(theta);
		
		// the position vector is the intersection point		
	}
	else if (r < kd + ks){
		// raio especular
		// direcao: R=2N(NL) - L
		direcao = kprod(2 * escalar(normal, toLight), normal);

	}
	else {
		// raio transmitido
		// TODO
		// objeto opaco? nenhuma cor transmitida
		// caso contrario... verificar refracao
	}

	// Use the direction and position vector to make a ray
	Raio novoRaio;
	novoRaio.direcao = direcao;
	novoRaio.posicao = posicao;

	Color recursion = trace_path(depth + 1, novoRaio, scene);

	// -----------------------------------output--------------------------------------


}

Raio cameraRay(int x, int y, Janela jan, Camera c){
	/* 
	* Trace a ray from the camera to the pixel (x,y) on the window 
	*/

	Raio output; // Output variable
	float xw, yw, zw, sizexw, sizeyw; // Pixel location and window size on world coordinates
	Vetor direcao, posicao; // Ray's direction and position

	sizexw = jan.x1 - jan.x0;
	sizeyw = jan.y1 - jan.y0;
	xw = ((float)x / jan.sizeX)*sizexw + jan.x0;
	yw = ((float)y / jan.sizeY)*sizeyw + jan.y0;
	zw = 0;

	posicao.x = c.x;
	posicao.y = c.y;
	posicao.z = c.z;
	
	direcao.x = xw - c.x;
	direcao.y = yw - c.y;
	direcao.z = zw - c.z;
	
	output.posicao = posicao;
	output.direcao = direcao;
	return output;
}

Color** render(Janela jan, Cena scene, Camera c){
	/*
	* Render a image using the path tracing algorithm for the given scene, camera and window.
	*/

	int xsize = jan.sizeX; // width in pixels
	int ysize = jan.sizeY; // height in pixels
	int nSamples = 5; // number of color samples per pixel
	Color** img; // output img
	Color sum, sample; // Acumulator and sample variables, used for each different pixel and pixel sample, respectively
	Raio ray; // Camera to window variable, used for each different pixel

	img = (Color**) malloc(sizeof(Color*)*xsize); // Array instanciation	

	//-----------------------------------------------MAIN LOOP----------------------------------------------
	// For each pixel, take nSample of colors and average them. The average is the color of that pixel.
	for (int i = 0; i < xsize; i++)
	{
		img[i] = (Color*) malloc(sizeof(Color)*ysize); // Array instanciation
		for (int j = 0; j < ysize; j++)
		{
			sum.r = 0;
			sum.g = 0;
			sum.b = 0;
			ray = cameraRay(i, j, jan, c);
			for (int k = 0; k < nSamples; k++)
			{
				sample = trace_path(0, ray, scene);
				sum = csum(sum, sample);				
			}
			img[i][j] = Color(sum.r/nSamples, sum.g/nSamples, sum.b/nSamples);
		}
	}

	return img;
}

int main(int argc, char **argv)
{
	//Loading camera paramenters
	Camera camera;

	//Loading view paramenters
	Janela janela;

	//Loading scene paramenters
	Cena cena;

	//Loading illumination paramenters
	Luz luz;

	//Loading objects of the scene
	vector<Objeto> objetos;
	vector<Vertice> vertices;
	vector<Face> faces;

	//Lendo arquivo sdl que descreve a cena utilizada e calculando a normal após
	lerCena("cornel_box\\cornellroom.sdl",camera,cena,janela,luz,objetos);

	for (int i = 0; i < objetos.size(); i++)
	{
		char realPath [100]= "cornel_box\\";
		strcat(realPath, objetos.at(i).path);
		lerObjeto(realPath, objetos.at(i).vertices, objetos.at(i).faces);
		objetos.at(i).normalFaces();
	}


	//Initiating glut variables
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(500, 500);//optional
	glutInitWindowSize(800, 600); //optional
	glutCreateWindow("OpenGL First Window");
	glEnable(GL_DEPTH_TEST);

	// register callbacks
	glutDisplayFunc(renderScene);

	glutMainLoop();

	
	return 0;
}

