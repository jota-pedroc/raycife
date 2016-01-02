#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cmath>
#include "../raycife/Lib/glew/glew.h"
#include "../raycife/Lib/freeglut/freeglut.h"

# define PI           3.14159265358979323846

using namespace std;


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
	Vetor toVetor();
	

};

class Ponto{
public:
	float x, y, z;

	//Ponto::Ponto(float x, float y, float z){
	//	this->x = x;
	//	this->y = y;
	//	this->z = z;
	//}

	//Ponto::~Ponto(){}
};

class Camera{
public:
	float x, y, z;

	/*Camera::Camera(float x, float y, float z){
	this->x = x;
	this->y = y;
	this->z = z;
	}

	Camera::~Camera(){}*/
};

class Cena{
public:
	Color background;

	Cena::Cena(float r, float g, float b){
		background.r = r;
		background.g = g;
		background.r = r;
	}

};

class Janela{
public:
	float x0, y0, x1, y1, sizeX, sizeY;
};

class Luz{
public:
	Ponto ponto;
	Color cor;
};

class Vertice{
public:
	float x, y, z;
};

class Face{
public:
	int v1, v2, v3;

	//Triangulo::Triangulo(int v1, int v2, int v3){
	//	this->v1 = v1;
	//	this->v2 = v2;
	//	this->v3 = v3;
	//}
};

class Objeto{
public:
	Color cor;
	float ka, kd, ks, kt, coeficienteEspecular;
	vector<Vertice> vertices;
	vector<Face> triangulos;

	void addVertice(float xV, float yV, float zV){
		Vertice vertice;
		vertice.x = xV;
		vertice.y = yV;
		vertice.z = zV;
		//adiciona um vertice a lista de vertices do objeto
		vertices.push_back(vertice);
	}

	void addTriangulo(int v1, int v2, int v3){
		Face triangulo;
		triangulo.v1 = v1;
		triangulo.v2 = v2;
		triangulo.v3 = v3;

		//adiciona um triangula a lista de triangulos do objeto
		triangulos.push_back(triangulo);
	}

};

class Vetor{
public:
	float x;
	float y;
	float z;
};

class Raio{
public:
	Vetor posicao;
	Vetor direcao;
	int tamanho;//TODO: necessario?
};



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

		int jj = 0;
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0){
			Vertice vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
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





