#include "stdafx.h"
#include "Vetor.h"

float Vetor::norma(){
	float a = this->x*this->x;
	float b = this->y*this->y;
	float c = this->z*this->z;

	return sqrt(a + b + c);
	
}


// Soma de vetores
Vetor vsum(Vetor v1, Vetor v2){
	Vetor out;
	out.x = v1.x + v2.x;
	out.y = v1.y + v2.y;
	out.z = v1.z + v2.z;
	return out;
}

//Funcoes matematicas importantes

// Produto escalar x vetor
Vetor kprod(float k, Vetor c){
	Vetor out;
	out.x = c.x*k;
	out.y = c.y*k;
	out.z = c.z*k;
	return out;
}

Vetor subVetor(Vetor v1, Vetor v2){
	Vetor out;

	out.x = v1.x - v2.x;
	out.y = v1.y - v2.y;
	out.z = v1.z - v2.z;
	return out;
}

//Definindo um vetor a partir de dois pontos
Vetor defVetor(Ponto ponto1, Ponto ponto2){
	Vetor retorno;

	retorno.x = ponto2.x - ponto1.x;
	retorno.y = ponto2.y - ponto1.y;
	retorno.z = ponto2.z - ponto1.z;

	return retorno;
};


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