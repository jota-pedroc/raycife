#include "stdafx.h"
#include "Cena.h"


bool lerCena(const char* path, Cena &cena){

	//O método abaixo foi baseado no cógigo encontrado no tutorial de OpenGL:
	//http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/

	FILE * file = fopen(path, "r");
	if (file == NULL){
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	while (1){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);

		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader

		if (strcmp(lineHeader, "eye") == 0){
			fscanf(file, "%f %f %f\n", &cena.olho.x, &cena.olho.y, &cena.olho.z);
		}
		else if (strcmp(lineHeader, "size") == 0){
			fscanf(file, "%f %f\n", &cena.janela.sizeX, &cena.janela.sizeY);

		}
		else if (strcmp(lineHeader, "ortho") == 0){
			fscanf(file, "%f %f %f %f\n", &cena.janela.x0, &cena.janela.y0, &cena.janela.x1, &cena.janela.y1);
		}
		else if (strcmp(lineHeader, "background") == 0){
			fscanf(file, "%f %f %f\n", &cena.background.r, &cena.background.g, &cena.background.b);
		}
		else if (strcmp(lineHeader, "ambient") == 0){
			fscanf(file, "%f\n", &cena.ambiente);
		}
		else if (strcmp(lineHeader, "light") == 0){
			Objeto o;
			fscanf(file, "%s %f %f %f %f\n", &o.path, &cena.luz.cor.r, &cena.luz.cor.g, &cena.luz.cor.b, &cena.luz.Ip);
			cena.objetos.push_back(o);
			cena.luz.objeto = &o;
		}
		else if (strcmp(lineHeader, "npaths") == 0){
			fscanf(file, "%i", &cena.npaths);
		}
		else if (strcmp(lineHeader, "tonemapping") == 0){
			fscanf(file, "%f", &cena.tonemapping);
		}
		else if (strcmp(lineHeader, "seed") == 0){
			fscanf(file, "%i", &cena.seed);
		}
		else if (strcmp(lineHeader, "object") == 0){
			Objeto o;
			fscanf(file, "%s %f %f %f %f %f %f %f %f %f\n", &o.path, &o.cor.r, &o.cor.g, &o.cor.b, &o.ka, &o.kd, &o.ks, &o.kt, &o.coeficienteEspecular);
			cena.objetos.push_back(o);
		}
		else if (strcmp(lineHeader, "quadric") == 0){
			Quadrica q;
			fscanf(file, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n", &q.a, &q.b, &q.c, &q.d, &q.e, &q.f, &q.g, &q.h, &q.j, &q.k, &q.cor.r, &q.cor.g, &q.cor.b,
				&q.ka, &q.kd, &q.ks, &q.kt, &q.coeficienteEspecular);
			cena.quadricas.push_back(q);
		}
		else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	return true;
}