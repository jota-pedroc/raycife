#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cmath>
#include "../raycife/Lib/glew/glew.h"
#include "../raycife/Lib/freeglut/freeglut.h"
#include "quadric.h"

# define PI           3.14159265358979323846


using namespace std;

class Vetor{
public:
	float x, y, z;
};

class Ponto{
public:
	float x, y, z;

};

class Vertice{
public:
	Ponto ponto;
};

class Face{
public:
	int v1, v2, v3;
	Vetor n1, n2, n3;
};


class Color{
public:
	float r, g, b;
	Color::Color(){
		this->r = 0;
		this->g = 0;
		this->b = 0;
	}
	Color::Color(float r, float g, float b){
		this->r = r;
		this->g = g;
		this->b = b;
	}
	Color::Color(Vetor v){
		this->r = v.x;
		this->g = v.y;
		this->b = v.z;
	}
	Vetor toVetor();
};

class Olho{
public:
	float x, y, z;
};


class Cena{
public:
	Color background;
	float ambiente;
	int npaths;
	float tonemapping;
	int seed;
};

class Janela{
public:
	float x0, y0, x1, y1, sizeX, sizeY;
};

class Luz{
public:
	Ponto ponto;
	Color cor;
	float Ip;
};



class Raio{
public:
	Vetor posicao;
	Vetor direcao;
	int tamanho;//TODO: necessario?

	Ray toRay();
};

Ray Raio::toRay(){
	Ray out;
	out.dir.x = this->direcao.x;
	out.dir.y = this->direcao.y;
	out.dir.z = this->direcao.z;
	out.org.x = this->posicao.x;
	out.org.y = this->posicao.y;
	out.org.z = this->posicao.z;
	out.depth = this->tamanho;
	return out;
}

Vetor Color::toVetor(){
	Vetor out;
	out.x = this->r;
	out.y = this->g;
	out.z = this->b;

	return out;
}

// Soma de cores
Color csum(Color c1, Color c2){
	Color output;
	output.r = c1.r + c2.r;
	output.g = c1.g + c2.g;
	output.b = c1.b + c2.b;
	return output;
}


// Soma de vetores
Vetor vsum(Vetor v1, Vetor v2){
	Vetor out;
	out.x = v1.x + v2.x;
	out.y = v1.y + v2.y;
	out.z = v1.z + v2.z;
	return out;
}

//Funcoes matematicas importantes

// Produto escalar x vetor
Vetor kprod(float k, Vetor c){
	Vetor out;
	out.x = c.x*k;
	out.y = c.y*k;
	out.z = c.z*k;
	return out;
}

Vetor subVetor(Vetor v1, Vetor v2){
	Vetor out;

	out.x = v1.x - v2.x;
	out.y = v1.y - v2.y;
	out.z = v1.z - v2.z;
	return out;
}

//Definindo um vetor a partir de dois pontos
Vetor defVetor(Ponto ponto1, Ponto ponto2){
	Vetor retorno;

	retorno.x = ponto2.x - ponto1.x;
	retorno.y = ponto2.y - ponto1.y;
	retorno.z = ponto2.z - ponto1.z;

	return retorno;
};


//Produto Escalar
float escalar(Vetor vetor1, Vetor vetor2){
	float  escalar = vetor1.x*vetor2.x + vetor1.y*vetor2.y + vetor1.z*vetor2.z;
	return escalar;
}

//Produto Vetorial
Vetor vetorial(Vetor vetor1, Vetor vetor2){

	Vetor resposta;
	resposta.x = (vetor1.y*vetor2.z) + (-1 * vetor1.z*vetor2.y);
	resposta.y = (vetor1.z*vetor2.x) + (-1 * vetor1.x*vetor2.z);
	resposta.z = (vetor1.x*vetor2.y) + (-1 * vetor1.y*vetor2.x);

	return resposta;
}

//Normalizar vetor
Vetor normalizar(Vetor vetor){

	float modulo = sqrt(vetor.x*vetor.x + vetor.y*vetor.y + vetor.z*vetor.z);

	Vetor vetorNormalizado = Vetor();
	vetorNormalizado.x = vetor.x / modulo;
	vetorNormalizado.y = vetor.y / modulo;
	vetorNormalizado.z = vetor.z / modulo;

	return vetorNormalizado;
}

//Calcular normal vertice de um triangulo (feito em relaçao a p1) *nao normalizado*
Vetor calcularNormal(Ponto p1, Ponto p2, Ponto p3){
	Vetor vetor1;
	Vetor vetor2;
	vetor1.x = p3.x - p1.x;
	vetor1.y = p3.y - p1.y;
	vetor1.z = p3.z - p1.z;

	vetor2.x = p2.x - p1.x;
	vetor2.y = p2.y - p1.y;
	vetor2.z = p2.z - p1.z;

	//cout<<"saiu calcula normal"<<endl;
	return vetorial(vetor1, vetor2);
}



class Objeto{
public:
	char path[100];
	Color cor;
	float ka, kd, ks, kt, coeficienteEspecular;
	vector<Vertice> vertices;
	vector<Face> faces;

	void normalVertice(){
		for (int i = 0; i < faces.size(); i++)
		{
			faces.at(i).n1 = calcularNormal(vertices.at(faces.at(i).v1 - 1).ponto, vertices.at(faces.at(i).v3 - 1).ponto, vertices.at(faces.at(i).v2 - 1).ponto);
			faces.at(i).n2 = calcularNormal(vertices.at(faces.at(i).v2-1).ponto, vertices.at(faces.at(i).v1-1).ponto, vertices.at(faces.at(i).v3-1).ponto);
			faces.at(i).n3 = calcularNormal(vertices.at(faces.at(i).v3 - 1).ponto, vertices.at(faces.at(i).v2 - 1).ponto, vertices.at(faces.at(i).v1 - 1).ponto);
		}
	}

	
	Vetor coordBaricentricas(Ponto p, Face f)
	{
		//Essa retorna alfa, beta e gama das coordenadas baricentricas
		//Código baseado no livro Christer Ericson's Real-Time Collision Detection
		float xa= vertices.at(f.v1 - 1).ponto.x;
		float xb = vertices.at(f.v2 - 1).ponto.x;
		float xc = vertices.at(f.v3 - 1).ponto.x;

		float ya = vertices.at(f.v1 - 1).ponto.y;
		float yb = vertices.at(f.v2 - 1).ponto.y;
		float yc = vertices.at(f.v3 - 1).ponto.y;

		float za = vertices.at(f.v1 - 1).ponto.z;
		float zb = vertices.at(f.v2 - 1).ponto.z;
		float zc = vertices.at(f.v3 - 1).ponto.z;

		Vetor v0, v1, v2;
		v0.x = xb-xa;
		v0.y = yb - ya;
		v0.z = zb - za;

		v1.x = xc - xa;
		v1.y = yc - ya;
		v1.z = zc - za;
		
		v2.x = p.x - xa;
		v2.y = p.y - ya;
		v2.z = p.z - za;

		float d00 = escalar(v0, v0);
		float d01 = escalar(v0, v1);
		float d11 = escalar(v1, v1);
		float d20 = escalar(v2, v0);
		float d21 = escalar(v2, v1);
		float denom = d00 * d11 - d01 * d01;
		float v = (d11 * d20 - d01 * d21) / denom;
		float w = (d00 * d21 - d01 * d20) / denom;
		float u = 1.0f - v - w;

		Vetor coorBar;
		coorBar.x = v;
		coorBar.y = w;
		coorBar.z = u;
		return coorBar;	
	}

	Vetor normalPonto(Ponto p, Face f){

		Vetor cBari = coordBaricentricas(p, f);

		Vetor normal;
		normal.x = f.n1.x*cBari.x + f.n2.x*cBari.y + f.n3.x*cBari.z;
		normal.y = f.n1.y*cBari.x + f.n2.y*cBari.y + f.n3.y*cBari.z;
		normal.z = f.n1.z*cBari.x + f.n2.z*cBari.y + f.n3.z*cBari.z;

		return normal;
	}

};

class Quadrica : public Objeto {
public:
	float a, b, c, d, e, f, g, h, j, k;

	Quad toQuad();
};

Quad Quadrica::toQuad(){
	Quad out;
	out.a = this->a;
	out.b = this->b;
	out.c = this->c;
	out.d = this->d;
	out.e = this->e;
	out.f = this->f;
	out.g = this->g;
	out.h = this->h;
	out.j = this->j;
	out.k = this->k;
	return out;
}

class Buffer{
public:

	Color** buffer;
};

Ponto pixelParaPonto2d(int i, int j, Janela janela) {
	//baseado no projeto Raycife de Ermano Arruda (eaa3)
	Ponto retorno;
	float larguraPixel;
	if (janela.x1 > janela.x0) larguraPixel = (janela.x1 - janela.x0) / janela.sizeX;
	else larguraPixel = (janela.x0 - janela.x1) / janela.sizeX;

	float alturaPixel;
	if (janela.y1 > janela.y0) alturaPixel = (janela.y1 - janela.y0) / janela.sizeY;
	else alturaPixel = (janela.y0 - janela.y1) / janela.sizeY;

	retorno.x = (janela.x0 + larguraPixel / 2) + (j * larguraPixel);
	retorno.y = (janela.y1 - alturaPixel / 2) - (i * alturaPixel);
	retorno.z = 0;

	return retorno;
}


bool lerCena(const char* path, Olho &olho, Cena &cena, Janela &janela, Luz &luz, vector<Objeto> &listaObjetos, vector<Quadrica> &listaQuadricas){

	//O método abaixo foi baseado no cógigo encontrado no tutorial de OpenGL:
	//http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/

	FILE * file = fopen(path, "r");
	if (file == NULL){
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	while (1){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);

		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader

		if (strcmp(lineHeader, "eye") == 0){
			fscanf(file, "%f %f %f\n", &olho.x, &olho.y, &olho.z);
		}
		else if (strcmp(lineHeader, "size") == 0){
			fscanf(file, "%f %f\n", &janela.sizeX, &janela.sizeY);
		
		}
		else if (strcmp(lineHeader, "ortho") == 0){
			fscanf(file, "%f %f %f %f\n", &janela.x0, &janela.y0, &janela.x1, &janela.y1);
		}
		else if (strcmp(lineHeader, "background") == 0){
			fscanf(file, "%f %f %f\n", &cena.background.r, &cena.background.g, &cena.background.b);
		}
		else if (strcmp(lineHeader, "ambient") == 0){
			fscanf(file, "%f\n", &cena.ambiente);
		}
		else if (strcmp(lineHeader, "light") == 0){
			Objeto o;
			fscanf(file, "%s %f %f %f %f\n", &o.path, &luz.cor.r, &luz.cor.g, &luz.cor.b, &luz.Ip);
			listaObjetos.push_back(o);
		}
		else if (strcmp(lineHeader, "npaths") == 0){
			fscanf(file, "%i", &cena.npaths);
		}
		else if (strcmp(lineHeader, "tonemapping") == 0){
			fscanf(file, "%f", &cena.tonemapping);
		}
		else if (strcmp(lineHeader, "seed") == 0){
			fscanf(file, "%i", &cena.seed);
		}
		else if (strcmp(lineHeader, "object") == 0){
			Objeto o;
			fscanf(file, "%s %f %f %f %f %f %f %f %f %f\n", &o.path, &o.cor.r, &o.cor.g, &o.cor.b, &o.ka, &o.kd, &o.ks, &o.kt, &o.coeficienteEspecular);
			listaObjetos.push_back(o);
		}
		else if (strcmp(lineHeader, "quadric") == 0){
			Quadrica q;
			fscanf(file, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n", &q.a, &q.b, &q.c, &q.d, &q.e, &q.f, &q.g, &q.h, &q.j, &q.k, &q.cor.r, &q.cor.g, &q.cor.b,
				&q.ka, &q.kd, &q.ks, &q.kt, &q.coeficienteEspecular);
			listaQuadricas.push_back(q);
		}
		else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	return true;
}


bool lerObjeto(const char* path, vector<Vertice> &out_vertices, vector<Face> &out_faces){
	//O método abaixo foi baseado no cógigo encontrado no tutorial de OpenGL:
	//http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/

	vector<Vertice> temp_vertices;
	vector<Face> vertexIndices;

	FILE * file = fopen(path, "r");
	if (file == NULL){
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	while (1){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);

		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0){
			Vertice vertex;
			fscanf(file, "%f %f %f\n", &vertex.ponto.x, &vertex.ponto.y, &vertex.ponto.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "f") == 0){
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3];
			int matches = fscanf(file, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);

			Face t;
			t.v1 = vertexIndex[0];
			t.v2 = vertexIndex[1];
			t.v3 = vertexIndex[2];
			vertexIndices.push_back(t);
		}
		else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	out_vertices = temp_vertices;
	out_faces = vertexIndices;

	return true;
}





