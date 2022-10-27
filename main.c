#include <SDL.h>
#include <SDL_ttf.h>
#include <stdlib.h>

SDL_Window *window;
SDL_Renderer *renderer;

void handleEvents();

SDL_Color fontColor = {255, 255, 255};
SDL_Color bg = {30, 30, 30};
SDL_Rect screenDimensions = {0, 0, 512, 512};

int left = 40;
int top = 40;

int inBetweenItems = 20;
int xStep = 20;

int squareLeftRightMargin = 7;
int squareSize = 8;
SDL_Color squareColor = {255, 255, 255};

typedef struct ItemView
{
	// top left corner of a squre in an item
	int x, y;
	char *text;
	SDL_Texture *textTexture;
} ItemView;

ItemView *view;
ItemView *view2;
ItemView *view3;
ItemView *view4;

TTF_Font *segoefont;
int ascent;

void renderItem(ItemView *itemView)
{
	// draw rect
	SDL_SetRenderDrawColor(renderer, squareColor.r, squareColor.g, squareColor.b, 255);
	SDL_Rect rectPosition = {itemView->x, itemView->y, squareSize, squareSize};
	SDL_RenderFillRect(renderer, &rectPosition);

	// draw text
	int textureWidth = 0;
	int textureHeight = 0;
	SDL_QueryTexture(itemView->textTexture, NULL, NULL, &textureWidth, &textureHeight);

	SDL_Rect text_dest = {0};
	text_dest.x = itemView->x + squareLeftRightMargin + squareSize;
	text_dest.y = itemView->y - (ascent / 2);
	text_dest.w = textureWidth;
	text_dest.h = textureHeight;

	SDL_RenderCopy(renderer, itemView->textTexture, NULL, &text_dest);
}

void draw()
{
	SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, 255);
	SDL_RenderFillRect(renderer, &screenDimensions);

	renderItem(view);
	renderItem(view2);
	renderItem(view3);
	renderItem(view4);

	SDL_RenderPresent(renderer);
}

ItemView *createItem(int x, int y, char *textChars)
{
	ItemView *res = malloc(sizeof(ItemView));
	res->x = x;
	res->y = y;

	res->text = textChars;
	SDL_Surface *text = TTF_RenderText_LCD(segoefont, res->text, fontColor, bg);
	res->textTexture = SDL_CreateTextureFromSurface(renderer, text);
	SDL_FreeSurface(text);
	return res;
}

int isRunning = 0;
void init()
{
	segoefont = TTF_OpenFont("fonts/seguisb.ttf", 16);
	ascent = TTF_FontAscent(segoefont);

	int yStepTotal = inBetweenItems + squareSize;
	int xStepTotal = xStep + squareSize;
	view = createItem(left, top, "Electro");
	view2 = createItem(left + xStepTotal, top + yStepTotal, "Carbon Based Lifeforms");
	view3 = createItem(left + xStepTotal, top + yStepTotal * 2, "Circular");
	view4 = createItem(left, top + yStepTotal * 3, "Ambient");
}

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();

	SDL_CreateWindowAndRenderer(screenDimensions.w, screenDimensions.h, 0, &window, &renderer);

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

	SDL_DestroyTexture(view->textTexture);

	TTF_Quit();
	SDL_Quit();

	free(view);

	return 0;
}

void handleEvents()
{
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
