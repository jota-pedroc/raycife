// ProjetoCG.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ProjetoCG.h"
#include <iostream>
#include <fstream>
#include <string>

Buffer buf;
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

//Loading objects of the scene
vector<Objeto> objetos;

///-------------------------OTHER PEOPLE'S STUFF-----------------------------------------///
//http://www.lighthouse3d.com/tutorials/maths/ray-triangle-intersection/
//http://www.cs.virginia.edu/~gfx/Courses/2003/ImageSynthesis/papers/Acceleration/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf

//// may be worth checking this one too: http://geomalgorithms.com/a06-_intersect-2.html
///* a = b - c */
//#define vector(a,b,c) \
//	(a)[0] = (b)[0] - (c)[0];	\
//	(a)[1] = (b)[1] - (c)[1];	\
//	(a)[2] = (b)[2] - (c)[2];
//
//int rayIntersectsTriangle(float *p, float *d,
//	float *v0, float *v1, float *v2) {
//
//	float e1[3], e2[3], h[3], s[3], q[3];
//	float a, f, u, v;
//	vector(e1, v1, v0);
//	vector(e2, v2, v0);
//
//	//crossProduct(h, d, e2);
//	//a = innerProduct(e1, h);
//
//	if (a > -0.00001 && a < 0.00001)
//		return(false);
//
//	f = 1 / a;
//	vector(s, p, v0);
//	//u = f * (innerProduct(s, h));
//
//	if (u < 0.0 || u > 1.0)
//		return(false);
//
//	//crossProduct(q, s, e1);
//	//v = f * innerProduct(d, q);
//
//	if (v < 0.0 || u + v > 1.0)
//		return(false);
//
//	// at this stage we can compute t to find out where
//	// the intersection point is on the line
//	//t = f * innerProduct(e2, q);
//
//	//if (t > 0.00001) // ray intersection
//		return(true);
//
//	//else // this means that there is a line intersection
//		// but not a ray intersection
//		return (false);
//
//}
/////------------------------- END OF OTHER PEOPLE'S STUFF-----------------------------------------///
//


Color difuso(float ip, float kd, Vetor lightDir, Vetor normal, Color corObjeto){

	Color retorno;

	retorno.r = ip*kd*escalar(lightDir, normal)*corObjeto.r; 
	retorno.g = ip*kd*escalar(lightDir, normal)*corObjeto.g;
	retorno.b = ip*kd*escalar(lightDir, normal)*corObjeto.b;
	
	return retorno;
};


float rand01(){
	return ((float)rand() / (RAND_MAX));
}

class Intersection {
public:
	Objeto objeto;
	Ponto p;
	Vetor normal;
private:
	Face f;
};

Intersection closestObject(Raio ray, Cena scene){
	// TODO
	Intersection MEMUDE;
	return MEMUDE;
}

Color trace_path(int depth, Raio ray, Cena scene, Luz luz){
	if (depth >= 5) return scene.background;

	Color output;

	// --------------------------check intersections--------------------------
	// ray intersects triangle for each triangle. Get the one closest to the eye that returns true.
	Intersection intersection = closestObject(ray, scene);
	Objeto closest = intersection.objeto;
	Ponto inters = intersection.p;
	Vetor normal = intersection.normal;
	Vetor toLight = defVetor(luz.ponto, inters);
	float kd = closest.kd, ks = closest.ks, kt = closest.kt;
	
	
	// ---------------------------color calculation----------------------------
	//Rambiente = Ia*kar
	float iA = scene.ambiente;
	Vetor ambiente = kprod(iA*closest.ka, closest.cor.toVetor());

	//Rdifuso = Ip*kd(L.N)r
	Color difusa = difuso(luz.Ip, closest.kd, toLight, normal, closest.cor);

	//Respecular = Ip*ks*(R.V)^n

	
	Vetor rVetor =subVetor(kprod(2 * escalar(normal, toLight), normal), toLight);
	rVetor = normalizar(rVetor);
	Vetor vVetor = kprod(-1, ray.direcao);
	vVetor = normalizar(vVetor);
	Color especular;
	float aux = pow(escalar(rVetor, vVetor), closest.coeficienteEspecular);
	aux = luz.Ip*closest.ks*aux;
	especular.r = luz.cor.r*aux;
	especular.g = luz.cor.g*aux;
	especular.b = luz.cor.b*aux;

	Color corLocal = csum(csum(difusa, Color(ambiente)), especular);


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
		direcao = subVetor(kprod(2 * escalar(normal, toLight), normal), toLight);

	}
	else {
		// raio transmitido
		// TODO
		// objeto opaco? nenhuma cor transmitida
		// caso contrario... verificar refracao
		depth = 5;
	}

	// Use the direction and position vector to make a ray
	Raio novoRaio;
	novoRaio.direcao = direcao;
//	novoRaio.posicao = posicao;

	Color recursion = trace_path(depth + 1, novoRaio, scene, luz);

	// -----------------------------------output--------------------------------------
	//*****Testar diferentes pesos*****
	output = csum(recursion, corLocal);
	output.r /= 2;
	output.g /= 2;
	output.b /= 2;
	return output;
}

Raio cameraRay(int x, int y, Janela jan, Olho o){
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

	posicao.x = o.x;
	posicao.y = o.y;
	posicao.z = o.z;
	
	direcao.x = xw - o.x;
	direcao.y = yw - o.y;
	direcao.z = zw - o.z;
	
	output.posicao = posicao;
	output.direcao = direcao;
	return output;
}

Color** render(Janela jan, Cena scene, Olho o, Luz luz){
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
			ray = cameraRay(i, j, jan, o);
			for (int k = 0; k < nSamples; k++)
			{
				sample = trace_path(0, ray, scene, luz);
				sum = csum(sum, sample);				
			}
			img[i][j] = Color(sum.r/nSamples, sum.g/nSamples, sum.b/nSamples);
		}
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
				glColor3ub(buf.buffer[i][j].r, buf.buffer[i][j].g, buf.buffer[i][j].b); 
				glVertex2d((i) / 400.f - 1, 1 - (j) / 300.f);
				glEnd();
		}
	}

	glFlush();
}

int main(int argc, char **argv)
{
	//Lendo arquivo sdl que descreve a cena utilizada e calculando a normal após
	lerCena("cornel_box\\cornellroom.sdl",olho,cena,janela,luz,objetos);

	//Carregando os objetos no vector de objetos
	for (int i = 0; i < objetos.size(); i++)
	{
		char realPath [100]= "cornel_box\\";
		strcat(realPath, objetos.at(i).path);
		lerObjeto(realPath, objetos.at(i).vertices, objetos.at(i).faces);
		objetos.at(i).normalVertice();
	}

	//Chamando o algoritmos de renderização que inclui o Path Tracing
	buf.buffer = render(janela,cena,olho,luz);



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

