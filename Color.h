#pragma once
#include "Vetor.h"
class Color
{
public:
	float r, g, b;

	Color();
	Color(float r, float g, float b);
	Color(Vetor v);

	Vetor toVetor();

	~Color();
};

// Soma de cores
Color csum(Color c1, Color c2);