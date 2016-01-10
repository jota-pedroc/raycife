#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "Color.h"
#include "Face.h"
#include "quadric.h"

using namespace std;

class Objeto{
public:
	char path[100];
	Color cor;
	float ka, kd, ks, kt, coeficienteEspecular;
	vector<Vertice> vertices;
	vector<Face> faces;

	void normalVertice(); // Pre-processamento. Calcula e armazena a normal de todos os vertices nas faces

	Vetor coordBaricentricas(Ponto p, Face f);

	Vetor normalPonto(Ponto p, Face f);

};



class Texture : public Objeto{
public:
	Color** buffer;
};

class Quadrica : public Objeto {
public:
	float a, b, c, d, e, f, g, h, j, k;

	Quad toQuad();
};


bool lerObjeto(const char* path, Objeto &objeto);
