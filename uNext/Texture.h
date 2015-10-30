#pragma once

#include <string>

class Texture
{
public:
	Texture(string fileName);
	~Texture(void);

	void Draw();
	void Draw(int offsetX, int offsetY);

	SDL_Texture* getTexture();
private:
	SDL_Texture* texture;


};

