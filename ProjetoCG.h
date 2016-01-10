#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cmath>
#include "../raycife/Lib/glew/glew.h"
#include "../raycife/Lib/freeglut/freeglut.h"
#include "quadric.h"
#include "Color.h"

# define PI           3.14159265358979323846


using namespace std;

class Buffer{
public:
	Color** buffer;
};


float distReta(Ponto p, Ponto p1, Ponto p2){
	Vetor AB = defVetor(p1,p2);
	Vetor PA=defVetor(p,p1);
	
	return (vetorial(PA, AB).norma())/AB.norma();
};








