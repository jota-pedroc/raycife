#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "../raycife/Lib/glew/glew.h"
#include "../raycife/Lib/freeglut/freeglut.h"



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


//Funcoes matematicas importantes

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


class Color{
public:
	float r, g, b;

};

class Camera{
public:
	float x, y, z;

};

class Cena{
public:
	Color background;
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


class Objeto{
public:
	Color cor;
	float ka, kd, ks, kt, coeficienteEspecular;
	vector<Vertice> vertices;
	vector<Face> faces;

	//void addVertice(float xV, float yV, float zV){
	//	Vertice vertice;
	//	vertice.x = xV;
	//	vertice.y = yV;
	//	vertice.z = zV;
	//	//adiciona um vertice a lista de vertices do objeto
	//	vertices.push_back(vertice);
	//}

	//void addTriangulo(int v1, int v2, int v3){
	//	Face triangulo;
	//	triangulo.v1 = v1;
	//	triangulo.v2 = v2;
	//	triangulo.v3 = v3;

	//	//adiciona um triangula a lista de triangulos do objeto
	//	triangulos.push_back(triangulo);
	//}

	void normalFaces(){
		for (int i = 0; i < faces.size(); i++)
		{
			faces.at(i).n1 = calcularNormal(vertices.at(faces.at(i).v1-1).ponto, vertices.at(faces.at(i).v2-1).ponto, vertices.at(faces.at(i).v3-1).ponto);
			faces.at(i).n2 = calcularNormal(vertices.at(faces.at(i).v2-1).ponto, vertices.at(faces.at(i).v1-1).ponto, vertices.at(faces.at(i).v3-1).ponto);
			faces.at(i).n3 = calcularNormal(vertices.at(faces.at(i).v3-1).ponto, vertices.at(faces.at(i).v1-1).ponto, vertices.at(faces.at(i).v2-1).ponto);
		}
	}

};

class Raio{
public:
	Ponto posicao;
	Ponto direcao;
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





