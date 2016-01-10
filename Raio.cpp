#include "stdafx.h"
#include "Raio.h"

Ray Raio::toRay(){
	Ray out;
	out.dir.x = this->direcao.x;
	out.dir.y = this->direcao.y;
	out.dir.z = this->direcao.z;
	out.org.x = this->posicao.x;
	out.org.y = this->posicao.y;
	out.org.z = this->posicao.z;
	out.depth = this->tamanho;
	return out;
}
