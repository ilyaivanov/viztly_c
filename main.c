#include <SDL.h>
#include <SDL_ttf.h>
#include <stdlib.h>

SDL_Window *window;
SDL_Renderer *renderer;

int isRunning = 0;

void handleEvents();

SDL_Texture *text_texture;
SDL_Rect text_dest = {0};

SDL_Color fontColor = {255, 255, 255};
SDL_Color bg = {30, 30, 30};
SDL_Rect screenDimensions = {0, 0, 512, 512};

int left = 40;
int top = 40;
int inBetweenItems = 20;

int squareLeftRightMargin = 7;
int squareSize = 8;
SDL_Color squareColor = {255, 255, 255};

void draw()
{
	SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, 255);
	SDL_RenderFillRect(renderer, &screenDimensions);

	SDL_SetRenderDrawColor(renderer, squareColor.r, squareColor.g, squareColor.b, 255);
	SDL_Rect rectPosition = {left + squareLeftRightMargin, top, squareSize, squareSize};
	SDL_RenderFillRect(renderer, &rectPosition);

	SDL_RenderCopy(renderer, text_texture, NULL, &text_dest);

	SDL_RenderPresent(renderer);
}

void init()
{
	TTF_Font *segoefont = TTF_OpenFont("fonts/seguisb.ttf", 16);

	SDL_Color txtBg = bg;
	SDL_Surface *text = TTF_RenderText_LCD(segoefont, "Electro", fontColor, txtBg);

	int ascent = TTF_FontAscent(segoefont);

	text_texture = SDL_CreateTextureFromSurface(renderer, text);

	text_dest.x = left + squareLeftRightMargin * 2 + squareSize;
	// text_dest.y = top - ((text->h - squareSize) / 2);
	text_dest.y = top - (ascent / 2);
	text_dest.w = text->w;
	text_dest.h = text->h;

	TTF_CloseFont(segoefont);

	SDL_FreeSurface(text);
}

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();

	SDL_CreateWindowAndRenderer(512, 512, 0, &window, &renderer);

	if (renderer)
	{
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		isRunning = 1;
		init();
	}

	while (isRunning)
	{
		handleEvents();
		draw();
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	TTF_Quit();
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
