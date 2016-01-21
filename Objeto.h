#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "Color.h"
#include "Face.h"
#include "quadric.h"

using namespace std;

class BoundingBox{
public:
	Ponto p1, p2, p3, p4;
	Ponto p5, p6, p7, p8;

	/*    p3_ _ _  p4
		   /     /|
	   p1 /_ _ _/ |
		 |   _ _|_| p8
	 	 | /    | /
	   p5|/_ _ _|/p6
	*/
	void fill(float gx, float lx, float gy, float ly, float gz, float lz);
	Ponto centro();
};

class Objeto{
public:
	char path[100];
	Color cor;
	float ka, kd, ks, kt, coeficienteEspecular;
	vector<Vertice> vertices;
	vector<Face> faces;
	BoundingBox boundingBox;
	bool isLight = false;
	bool isTexture = false;
	float coeficienteRefracao;


	void normalVertice(); // Pre-processamento. Calcula e armazena a normal de todos os vertices nas faces

	Vetor coordBaricentricas(Ponto p, Face f);

	Vetor normalPonto(Ponto p, Face f);

};

class Quadrica : public Objeto {
public:
	float a, b, c, d, e, f, g, h, j, k;

	Quad toQuad();
};


bool lerObjeto(const char* path, Objeto &objeto);
