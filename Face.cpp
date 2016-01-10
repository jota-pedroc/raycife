#include "stdafx.h"
#include "Face.h"

//Calcular normal vertice de um triangulo (feito em relaçao a p1) *nao normalizado*
Vetor calcularNormal(Vertice* p1, Vertice* p2, Vertice* p3){
	Vetor vetor1;
	Vetor vetor2;
	vetor1.x = p3->x - p1->x;
	vetor1.y = p3->y - p1->y;
	vetor1.z = p3->z - p1->z;

	vetor2.x = p2->x - p1->x;
	vetor2.y = p2->y - p1->y;
	vetor2.z = p2->z - p1->z;

	return vetorial(vetor1, vetor2);
}

void Face::calcularNormaisVertices(){
	this->n1 = calcularNormal(this->v1, this->v3, this->v2);
	this->n2 = calcularNormal(this->v2, this->v1, this->v3);
	this->n3 = calcularNormal(this->v3, this->v2, this->v1);
}
