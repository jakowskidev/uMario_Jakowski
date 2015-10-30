#include "Texture.h"
#include "header.h"

using namespace std;

/* ******************************************** */

Texture::Texture(string fileName) {
	//IMG_LoadTexture(renderer, fileName.c_str());

}

Texture::~Texture(void) {
	SDL_DestroyTexture(texture);
}

/* ******************************************** */

void Texture::Draw() { Texture::Draw(0, 0); }

void Texture::Draw(int offsetX, int offsetY) {

}

/* ******************************************** */

SDL_Texture* Texture::getTexture() {
	return texture;
}