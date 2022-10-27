#include <SDL.h>
#include <SDL_ttf.h>
#include <stdlib.h>

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Surface* surface;

int isRunning = 0;

void handleEvents();

SDL_Texture* text_texture;
SDL_Rect text_dest = { 0 };

void draw()
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect rect = { 20, 20, 40, 40 };
    SDL_RenderFillRect(renderer, &rect);

	SDL_RenderCopy(renderer, text_texture, NULL, &text_dest);

	SDL_RenderPresent(renderer);
}




int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();

	TTF_Font*  font = TTF_OpenFont("C:/Windows/Fonts/segoeui.ttf", 28);

	
    SDL_CreateWindowAndRenderer(512, 512, 0, &window, &renderer);

	if (renderer) {
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		isRunning = 1;

		text_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 400, 400);
		SDL_Surface* text;
		SDL_Color color = { 255, 255, 255 };
		SDL_Color bgcolor = { 0, 0, 0 };

		text = TTF_RenderText_LCD(font, "Hello, World!", color, bgcolor);

		text_texture = SDL_CreateTextureFromSurface(renderer, text);

		text_dest.x = 200;
		text_dest.y = 200;
		text_dest.w = text->w;
		text_dest.h = text->h;
	}


    while (isRunning)
    {
        handleEvents();
        draw();
    }

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window); 
	SDL_Quit();

	return 0;
}
// handles any events that SDL noticed.
void handleEvents()
{
	// the only event we'll check is the  SDL_QUIT event.
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type)
	{
	case SDL_QUIT:
		isRunning = 0;
		break;
	default:
		break;
	}
}
