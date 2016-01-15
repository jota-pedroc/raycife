#include "stdafx.h"
#include "Objeto.h"

void BoundingBox::fill(float gx, float lx, float gy, float ly, float gz, float lz){
	this->p1.x = lx;
	this->p1.y = gy;
	this->p1.z = gz;

	this->p2.x = gx;
	this->p2.y = gy;
	this->p2.z = gz;

	this->p3.x = lx;
	this->p3.y = gy;
	this->p3.z = lz;

	this->p4.x = gx;
	this->p4.y = gy;
	this->p4.z = lz;

	this->p5.x = lx;
	this->p5.y = ly;
	this->p5.z = gz;

	this->p6.x = gx;
	this->p6.y = ly;
	this->p6.z = gz;

	this->p7.x = lx;
	this->p7.y = ly;
	this->p7.z = lz;

	this->p8.x = gx;
	this->p8.y = ly;
	this->p8.z = lz;
}

Ponto BoundingBox::centro(){
	Ponto out;
	out.x = 0;
	out.y = 0;
	out.z = 0;
	return out;
}

void Objeto::normalVertice(){
	for (int i = 0; i < faces.size(); i++)
	{
		faces.at(i).calcularNormaisVertices();
	}
}


Vetor Objeto::coordBaricentricas(Ponto p, Face f)
{
	//Retorna alfa, beta e gama das coordenadas baricentricas
	//Código baseado no livro Christer Ericson's Real-Time Collision Detection
	float xa = f.v1->x;
	float xb = f.v2->x;
	float xc = f.v3->x;

	float ya = f.v1->y;
	float yb = f.v2->y;
	float yc = f.v3->y;

	float za = f.v1->z;
	float zb = f.v2->z;
	float zc = f.v3->z;

	Vetor v0, v1, v2;
	v0.x = xb - xa;
	v0.y = yb - ya;
	v0.z = zb - za;

	v1.x = xc - xa;
	v1.y = yc - ya;
	v1.z = zc - za;

	v2.x = p.x - xa;
	v2.y = p.y - ya;
	v2.z = p.z - za;

	float d00 = escalar(v0, v0);
	float d01 = escalar(v0, v1);
	float d11 = escalar(v1, v1);
	float d20 = escalar(v2, v0);
	float d21 = escalar(v2, v1);
	float denom = d00 * d11 - d01 * d01;
	float v = (d11 * d20 - d01 * d21) / denom;
	float w = (d00 * d21 - d01 * d20) / denom;
	float u = 1.0f - v - w;

	Vetor coorBar;
	coorBar.x = v;
	coorBar.y = w;
	coorBar.z = u;
	return coorBar;
}

Vetor Objeto::normalPonto(Ponto p, Face f){

	Vetor cBari = coordBaricentricas(p, f);

	Vetor normal;
	normal.x = f.n1.x*cBari.x + f.n2.x*cBari.y + f.n3.x*cBari.z;
	normal.y = f.n1.y*cBari.x + f.n2.y*cBari.y + f.n3.y*cBari.z;
	normal.z = f.n1.z*cBari.x + f.n2.z*cBari.y + f.n3.z*cBari.z;

	return normal;
}

Quad Quadrica::toQuad(){
	Quad out;
	out.a = this->a;
	out.b = this->b;
	out.c = this->c;
	out.d = this->d;
	out.e = this->e;
	out.f = this->f;
	out.g = this->g;
	out.h = this->h;
	out.j = this->j;
	out.k = this->k;
	return out;
}

bool lerObjeto(const char* path, Objeto &objeto){
	//O método abaixo foi baseado no cógigo encontrado no tutorial de OpenGL:
	//http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/

	FILE * file = fopen(path, "r");
	if (file == NULL){
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}
	
	float gx = INT_MIN, lx = INT_MAX, gy = INT_MIN, ly = INT_MAX, gz = INT_MIN, lz = INT_MAX;


	while (1){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);

		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0){
			Vertice vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			objeto.vertices.push_back(vertex);

			if (vertex.x > gx) gx = vertex.x;
			if (vertex.x < lx) lx = vertex.x;
			if (vertex.y > gy) gy = vertex.y;
			if (vertex.y < ly) ly = vertex.y;
			if (vertex.z > gz) gz = vertex.z;
			if (vertex.z < lz) lz = vertex.z;
		}
		else if (strcmp(lineHeader, "f") == 0){
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3];
			int matches = fscanf(file, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);

			Face t;
			t.v1Index = vertexIndex[0];
			t.v2Index = vertexIndex[1];
			t.v3Index = vertexIndex[2];
			objeto.faces.push_back(t);
		}
		else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	for (size_t i = 0; i < objeto.faces.size(); i++)
	{
		Face &currentFace = objeto.faces.at(i);
		currentFace.v1 = &objeto.vertices.at(currentFace.v1Index - 1);
		currentFace.v2 = &objeto.vertices.at(currentFace.v2Index - 1);
		currentFace.v3 = &objeto.vertices.at(currentFace.v3Index - 1);
	}

	objeto.boundingBox.fill(gx, lx, gy, ly, gz, lz);

	return true;
}