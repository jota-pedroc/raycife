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

/////-------------------------OTHER PEOPLE'S STUFF-----------------------------------------///
////http://www.lighthouse3d.com/tutorials/maths/ray-triangle-intersection/
////http://www.cs.virginia.edu/~gfx/Courses/2003/ImageSynthesis/papers/Acceleration/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
//
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
//int* pixel(Camera c, Ponto p, Janela j){
//	/* dado um ponto qualquer na cena, em qual pixel o observador o enxerga?
//	* R: ponto de interseção do raio Ponto-Camera (reta r) com a janela
//	* 
//	* São paralelos? Se sim, não visível. (Provavelmente não vai acontecer se a janlea/camera estiverem configurados certo)
//	* Caso contrario:
//	* Reta r: P = P + t(C-P) 
//	* (x,y,z) = (x0,y0,z0) + t(a,b,c)
//
//	* sabemos que a janela tem z = 0, como queremos a interseção, podemos substituir:
//	* z = z0 + tc -> 0 = z0 + tc -> t = (-z0)/c
//	* Dai tem-se que x e y são:
//	* x = x0 + ta
//	* y = y0 + tb
//	*
//	* See x e y estiverem dentro da janela, o ponto é visivel e x,y são as coordenadas dele na tela.
//	*/
//
//	float t, x, y;
//	t = -(p.z) / (c.z - p.z);
//	x = p.x + t*(c.x - p.x);
//	y = p.y + t*(c.y - p.y);
//
//	if ((x >= j.x0 && x <= j.x1) && (y >= j.y0 && y <= j.y1)){
//		// visivel
//		int output[2] = {int(x), int(y)};
//		return output;
//	}
//	else {
//		// nao visivel
//		int output[2] = { -1, -1 };
//		return output;
//	}
//
//}
//
//Color prod(float k, Color c){
//
//}
//
//Color difuso(float ip, float kd, Ponto lightDir, Ponto normal){
//
//}
//
//Ponto vdif(Ponto p, Ponto q){
//
//}
//
//Color* trace_path(int depth, Raio* ray, Cena* scene){
//	if (depth == 0) return &(scene->background);
//
//	Color* output = nullptr;
//
//	// --------------------------check intersections--------------------------
//	// ray intersects triangle for each triangle. Get the one closest to the eye that returns true.
//	Objeto closest;
//	Ponto inters;
//	// ---------------------------color calculation----------------------------
//	float iA;
//	// TODO: Ia = scene->ambiente
//	Color ambiente = prod(iA*closest.ka, closest.cor);
//	Luz l;
//	// TODO:l = scene.luz
//	int lintensidade; Ponto normal;// TODO : temp
//
//	Color difusa = difuso(lintensidade, closest.kd, vdif(l.ponto, inters), normal);
//
//
//	// -------------------------recursion for contribution on other objects---------------------------------
//	
//
//	// -----------------------------------output--------------------------------------
//}

//for (int i = 0; i < arq->size[0]; i++){
//	for (int j = 0; j < arq->size[1]; j++){
//		//criação do raio que sai do olho e passa pelo ponto relativo ao pixel atual
//		Ray ray;
//		ray.depth = 0;
//		ray.org = arq->eye;
//		ray.dir = vsub(pixelParaPonto2d(i, j), arq->eye);
//
//		if (i >= 90 && i <= 110 && j >= 80 && j <= 120) {
//			int teste = 0;
//		}
//
//		//ray trace no pixel atual retornando sua cor para ser pintada na tela
//		tela[(i*arq->size[1]) + j] = rayTrace(ray);
//	}
//}

void percorrerTela(){
	//Baseado no RayTracing de Ermano
	
	Ponto o;
	o.x = olho.x;
	o.y = olho.y;
	o.z = olho.z;

	for (int i = 0; i < janela.sizeX; i++)
	{
		for (int j = 0; j < janela.sizeY; j++)
		{
			Raio raio;
			raio.posicao = o;
			raio.direcao = defVetor(pixelParaPonto2d(i,j,janela), o);
		}
	}

	//Lançar raio para cada pixel n vezes e armazenar no buffer
	//Verificar se a ordem dos pixels está correta
	//buffer[j][i] = pathTracing(); 

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
	for (int i = 0; i < 800; i++)
	{
		//pixel height
		for (int j = 0; j < 600; j++)
		{
				glBegin(GL_POINTS);
				glColor3ub(buf.buffer[i][j].r, buf.buffer[i][j].g, buf.buffer[i][j].b); //Modificar para receber do path tracing no buffer
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

	for (int i = 0; i < objetos.size(); i++)
	{
		char realPath [100]= "cornel_box\\";
		strcat(realPath, objetos.at(i).path);
		lerObjeto(realPath, objetos.at(i).vertices, objetos.at(i).faces);
		objetos.at(i).normalVertice();
	}

	////Inicializando o buffer que ira conter a cor de cada pixel (mudar para ser o background ou nada)
	//for (int  i = 0; i < 800; i++)
	//{
	//	for (int j = 0; j < 600; j++)
	//	{
	//		if (i<400){
	//			buf.buffer[i][j].r = 255;
	//			buf.buffer[i][j].g = 255;
	//			buf.buffer[i][j].b = 255;
	//		}
	//		else{
	//			buf.buffer[i][j].r = 0;
	//			buf.buffer[i][j].g = 0;
	//			buf.buffer[i][j].b = 0;
	//		}
	//	}
	//}

	percorrerTela();

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

	
	return 0;
}

