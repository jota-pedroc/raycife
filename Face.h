#pragma once
#include "Vetor.h"
class Face{
public:
	Vertice *v1, *v2, *v3; // Ponteiros para os vertices desse triangulo
	Vetor n1, n2, n3; // Normais de cada vertice desse triangulo
	int v1Index, v2Index, v3Index;

	void calcularNormaisVertices();
};
