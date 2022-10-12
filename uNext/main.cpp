#include "header.h"
#include "Core.h"
#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif

#ifdef __EMSCRIPTEN__
CCore oCore;
void main_loop()
{
	oCore.mainLoop();
}
#endif

int main(int argc, char *argv[])
{
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, 1);
#else
	CCore oCore;

	oCore.mainLoop();
#endif

	return 0;
}
