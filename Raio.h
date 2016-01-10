#pragma once
#include "Vetor.h"
#include "quadric.h"

class Raio{
public:
	Vetor posicao;
	Vetor direcao;
	int tamanho;//TODO: necessario?

	Ray toRay();
};

