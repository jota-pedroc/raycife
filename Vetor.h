#pragma once
#include <cmath>
#include "Ponto.h"

class Vetor{
public:
	float x, y, z;

	float norma();
};

Vetor defVetor(Ponto ponto1, Ponto ponto2); // Definir um vetor a partir de dois pontos. Resultado aponta de ponto1 para ponto2

Vetor kprod(float k, Vetor c); // Produto de escalar por vetor
Vetor vsum(Vetor v1, Vetor v2); // Soma de vetores
Vetor subVetor(Vetor v1, Vetor v2); // Subtracao de vetores
Vetor vetorial(Vetor vetor1, Vetor vetor2); // Produto vetorial v1 x v2
float escalar(Vetor vetor1, Vetor vetor2); // Produto interno  <v1.v2>
Vetor normalizar(Vetor vetor); // Normalizar vetor. |vetor| = 1