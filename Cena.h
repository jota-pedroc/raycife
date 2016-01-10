#pragma once
#include <vector>

#include "Color.h"
#include "Objeto.h"

class Olho{
public:
	float x, y, z;
};

class Janela{
public:
	float x0, y0, x1, y1, sizeX, sizeY;
};

class Luz {
public:
	Ponto ponto;
	Color cor;
	float Ip;
	Objeto* objeto;
};

class Cena{
public:
	Color background;
	float ambiente;
	int npaths;
	float tonemapping;
	int seed;

	Luz luz;
	vector<Objeto> objetos;
	vector<Quadrica> quadricas;
	Olho olho;
	Janela janela;
};



bool lerCena(const char* path, Cena &cena);