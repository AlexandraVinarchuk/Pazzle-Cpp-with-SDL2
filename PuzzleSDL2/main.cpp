#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <Windows.h>
#include <iostream>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

class TextureEx
{
private:
	std::string _path; //название пути
	SDL_Rect _position; //позиция
	SDL_Texture* _texture; //текстура
public:
	//конструктор
	TextureEx(string path, SDL_Rect position, SDL_Texture* texture)
	{
		_path = path;
		_position = position;
		_texture = texture;
	}
	//возврат
	SDL_Texture* GetTexture()
	{
		return _texture;
	}
	SDL_Rect GetPosition()
	{
		return _position;
	}
	std::string GetPath()
	{
		return _path;
	}
};

class Piece
{
private:
	int _number; //номер фрагмента
	bool _isPlaced; //проверка установки
	SDL_Rect _source; //выбранный фрагмент
	SDL_Rect _destination; //координаты фрагмента

public:
	//конструктор 
	Piece() {}
	//конструктор с параметрами
	Piece(SDL_Rect source, SDL_Rect destination, int number, int isPlaced)
	{
		_number = number;
		_isPlaced = isPlaced;
		_source = source;
		_destination = destination;
	}
	//нумерация фрагмента
	void SetPieceNumber(int number, int x, int y, int size)
	{
		_number = number;
		_source.x = x;
		_source.y = y;
		_source.w = size;
		_source.h = size;
	}
	//установлен ли фрагмент паззла на место
	void SetPieceIsPlaced(bool isPlaced)
	{
		_isPlaced = isPlaced;
	}
	//определение координат фрагмента
	void SetPieceDestination(int x, int y, int w, int h)
	{
		_destination.x = x;
		_destination.y = y;
		_destination.w = w;
		_destination.h = h;
	}

	void SetPieceDestination(SDL_Rect rect)
	{
		_destination.x = rect.x;
		_destination.y = rect.y;
		_destination.w = rect.w;
		_destination.h = rect.h;
	}
	//возврат
	int GetPieceNumber()
	{
		return _number;
	}
	bool GetPieceIsPlaced()
	{
		return _isPlaced;
	}

	SDL_Rect GetPieceSource()
	{
		return _source;
	}

	SDL_Rect GetPieceDestination()
	{
		return _destination;
	}
};

int hResolution = 1024;
int vResolution = 700;

int hCount = 4;
int vCount = 3;
int pSize = 150;

int hBackOffset = (hResolution - hCount * pSize) / 2;
int vBackOffset = (vResolution - vCount * pSize) / 2;

SDL_Window * window;
SDL_Renderer * renderer;
//SDL_Surface * image;
std::vector<Piece*> vSprite;
SDL_Texture * texture;
std::vector<TextureEx*> vImages;

int getRandomNumber(int min, int max)
{
	static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
	// Равномерно распределяем рандомное число в нашем диапазоне
	return static_cast<int>(rand() * fraction * (max - min + 1) + min);
}

bool IsComplete(std::vector<Piece*> &vSprite)
{
	bool result = true;
	for (int i = 0; i < hCount*vCount; i++)
	{
		if (!(*vSprite[i]).GetPieceIsPlaced())
		{
			result = false;
			break;
		}
	}
	return result;
}

int ShowCompleteScreen()
{
	SDL_Event event;
	bool quit = false;
	SDL_Rect backRectSource;
	backRectSource.x = 0;
	backRectSource.y = 0;
	backRectSource.w = hCount * pSize;
	backRectSource.h = vCount * pSize;
	SDL_Rect backRectDestination;
	backRectDestination.x = hBackOffset;
	backRectDestination.y = vBackOffset;
	backRectDestination.w = hCount * pSize;
	backRectDestination.h = vCount * pSize;
	SDL_RenderCopy(renderer, texture, &backRectSource, &backRectDestination);
	SDL_Surface* bContinue = IMG_Load("Resource\\Continue.png");
	SDL_Texture* texContinue = SDL_CreateTextureFromSurface(renderer, bContinue);
	SDL_Rect bPosContinue;
	bPosContinue.x = hResolution / 2 - 150;
	bPosContinue.y = vResolution - 155;
	bPosContinue.w = 300;
	bPosContinue.h = 150;
	SDL_RenderCopy(renderer, texContinue, NULL, &bPosContinue);
	SDL_RenderPresent(renderer);
	SDL_RenderClear(renderer);
	while (!quit)
	{
		while (SDL_PollEvent(&event) > 0)
		{
			if (event.type == SDL_QUIT) quit = true;

			if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				SDL_Point tempMouse;
				tempMouse.x = event.button.x;
				tempMouse.y = event.button.y;
				if (SDL_PointInRect(&tempMouse, &bPosContinue))
				{
					return 2;
				}
			}
		}
	}
	return 0;
}

void DrawPieces(std::vector<Piece*> &vSprite)
{
	//=== Рисуем сетку
	for (int i = 0; i < hCount*vCount; i++)
	{
		SDL_SetRenderDrawColor(renderer, 184, 255, 43, 1);
		SDL_Rect rect;
		rect.x = pSize * (i % hCount) + hBackOffset;
		rect.y = pSize * (i / hCount) + vBackOffset;
		rect.w = pSize;
		rect.h = pSize;
		SDL_RenderFillRect(renderer, &rect);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 1);
		rect.x += 1;
		rect.y += 1;
		rect.w -= 2;
		rect.h -= 2;
		SDL_RenderFillRect(renderer, &rect);
	}
	//=== Рисуем установленные спрайты
	for (int i = 0; i < hCount*vCount; i++)
	{
		Piece piece = *vSprite[i];
		if (piece.GetPieceIsPlaced())
			SDL_RenderCopy(renderer, texture, &piece.GetPieceSource(), &piece.GetPieceDestination());
	}
	//=== Рисуем неустановленные спрайты
	for (int i = 0; i < hCount*vCount; i++)
	{
		Piece piece = *vSprite[i];
		if (!piece.GetPieceIsPlaced())
			SDL_RenderCopy(renderer, texture, &piece.GetPieceSource(), &piece.GetPieceDestination());
	}
}
// формирвание текстуры, указанного размера, из изображения, находящегося по указанному пути 
SDL_Texture * get_texture(SDL_Renderer * pRenderer, std::string image_filename, int limit)
{
	SDL_Texture * result = NULL;

	SDL_Surface * pSurface = IMG_Load(image_filename.c_str());

	if (pSurface == NULL)
	{
		printf("Error image load: %s\n", IMG_GetError());
	}
	else
	{
		int width = pSurface->w;
		int height = pSurface->h;

		if ((width > limit) || (height > limit))
		{
			SDL_Rect sourceDimensions;
			sourceDimensions.x = 0;
			sourceDimensions.y = 0;
			sourceDimensions.w = width;
			sourceDimensions.h = height;

			float scale = (float)limit / (float)width;
			float scaleH = (float)limit / (float)height;

			if (scaleH < scale) scale = scaleH;

			SDL_Rect targetDimensions;
			targetDimensions.x = 0;
			targetDimensions.y = 0;
			targetDimensions.w = (int)(width * scale);
			targetDimensions.h = (int)(height * scale);

			// create a 32 bits per pixel surface to Blit the image to first, before BlitScaled
			SDL_Surface *p32BPPSurface = SDL_CreateRGBSurface(
				pSurface->flags,
				sourceDimensions.w,
				sourceDimensions.h,
				32,
				pSurface->format->Rmask,
				pSurface->format->Gmask,
				pSurface->format->Bmask,
				pSurface->format->Amask);

			if (SDL_BlitSurface(pSurface, NULL, p32BPPSurface, NULL) < 0)
			{
				printf("Error did not blit surface: %s\n", SDL_GetError());
			}
			else
			{
				// create another 32 bits per pixel surface are the desired scale
				SDL_Surface *pScaleSurface = SDL_CreateRGBSurface(
					p32BPPSurface->flags,
					targetDimensions.w,
					targetDimensions.h,
					p32BPPSurface->format->BitsPerPixel,
					p32BPPSurface->format->Rmask,
					p32BPPSurface->format->Gmask,
					p32BPPSurface->format->Bmask,
					p32BPPSurface->format->Amask);

				// 32 bit per pixel surfaces (loaded from the original file) won't scale down with BlitScaled, suggestion to first fill the surface
				// 8 and 24 bit depth pngs did not require this
				SDL_FillRect(pScaleSurface, &targetDimensions, SDL_MapRGBA(pScaleSurface->format, 255, 0, 0, 255));

				if (SDL_BlitScaled(p32BPPSurface, NULL, pScaleSurface, NULL) < 0)
				{
					printf("Error did not scale surface: %s\n", SDL_GetError());

					SDL_FreeSurface(pScaleSurface);
					pScaleSurface = NULL;
				}
				else
				{
					SDL_FreeSurface(pSurface);

					pSurface = pScaleSurface;
					width = pSurface->w;
					height = pSurface->h;
				}
			}

			SDL_FreeSurface(p32BPPSurface);
			p32BPPSurface = NULL;
		}

		SDL_Texture * pTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);

		if (pTexture == NULL) {
			printf("Error image load: %s\n", SDL_GetError());
		}
		else {
			SDL_SetTextureBlendMode(
				pTexture,
				SDL_BLENDMODE_BLEND);

			result = pTexture;
		}

		SDL_FreeSurface(pSurface);
		pSurface = NULL;
	}

	return result;
}

void RecalcPole(SDL_Point PoleResolution, std::vector<Piece*> &vSprite)
{
	int hResolutionPrevious = hResolution;
	int vResolutionPrevious = vResolution;
	hResolution = PoleResolution.x;
	vResolution = PoleResolution.y;

	int pSizePrevious = pSize;
	pSize = hResolution / (hCount + 2);

	int hBackOffsetPrevious = hBackOffset;
	int vBackOffsetPrevious = vBackOffset;
	hBackOffset = (hResolution - hCount * pSize) / 2;
	vBackOffset = (vResolution - vCount * pSize) / 2;

	for (int i = 0; i < vCount*hCount; i++)
	{
		int x = i % hCount;
		int y = i / hCount;
		vSprite.push_back(new Piece());
		vSprite.back()->SetPieceDestination(getRandomNumber(0, hResolution - pSize), getRandomNumber(0, vResolution - pSize), pSize, pSize);
		vSprite.back()->SetPieceNumber(i, x*pSize, y*pSize, pSize);
		vSprite.back()->SetPieceIsPlaced(false);//признак того, что фрагмент установлен на место
	}
}

void ShowStartScreen()
{
	TTF_Font* font = TTF_OpenFont("Resource\\MTCORSVA.TTF", 48);
	SDL_Color color = { 18, 255, 0 };
	static_assert(sizeof(wchar_t) == sizeof(Uint16));
	SDL_Surface* surface = TTF_RenderUNICODE_Blended(font, reinterpret_cast<Uint16 const *>(L"Курсовая работа"), color);
	SDL_Texture * texAuthor = SDL_CreateTextureFromSurface(renderer, surface);
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(texAuthor, NULL, NULL, &texW, &texH);
	SDL_Rect dstrect = { (hResolution - texW) / 2, 100, texW, texH };
	SDL_RenderCopy(renderer, texAuthor, NULL, &dstrect);

	font = TTF_OpenFont("Resource\\MTCORSVA.TTF", 72);
	surface = TTF_RenderUNICODE_Blended(font, reinterpret_cast<Uint16 const *>(L"PUZZLE GAME"), color);
	SDL_Texture * texGameName = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_QueryTexture(texGameName, NULL, NULL, &texW, &texH);
	dstrect = { (hResolution - texW) / 2, (vResolution - texH) / 2, texW, texH };
	SDL_RenderCopy(renderer, texGameName, NULL, &dstrect);
	//отрисовываем надписи
	SDL_RenderPresent(renderer);
	SDL_RenderClear(renderer);
	SDL_Delay(3000);
}

bool ShowRuleScreen()
{
	SDL_RenderClear(renderer);
	TTF_Font* font = TTF_OpenFont("Resource\\MTCORSVA.TTF", 32);
	SDL_Color color = { 18, 255, 0 };
	static_assert(sizeof(wchar_t) == sizeof(Uint16));
	SDL_Surface* surface = TTF_RenderUNICODE_Blended_Wrapped(font, reinterpret_cast<Uint16 const *>(L"Цель игры - расположить фрагменты картинки на правильные места, чтобы получилось цельное изображение. Управление в игре осуществляется с помощью мыши. Кликните на элемент паззла, чтобы выделить его. После этого фрагмент можно перетаскивать. Поместив фрагмент паззла на нужное место, отпустите кнопку мыши."), color, hResolution - 100);
	SDL_Texture * texRule = SDL_CreateTextureFromSurface(renderer, surface);
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(texRule, NULL, NULL, &texW, &texH);
	SDL_Rect rectRule = { (hResolution - texW) / 2, vResolution / 2 - texH - 50, texW, texH };
	SDL_RenderCopy(renderer, texRule, NULL, &rectRule);
	SDL_FreeSurface(surface);

	font = TTF_OpenFont("Resource\\MTCORSVA.TTF", 48);
	surface = TTF_RenderUNICODE_Blended(font, reinterpret_cast<Uint16 const *>(L"Назад"), color);
	SDL_Texture * texBack = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_QueryTexture(texBack, NULL, NULL, &texW, &texH);
	SDL_Rect rectBack = { (hResolution - texW) / 2,  vResolution - 100, texW, texH };
	SDL_RenderCopy(renderer, texBack, NULL, &rectBack);
	SDL_FreeSurface(surface);
	SDL_RenderPresent(renderer);
	
	SDL_Event event;
	bool isSelected = false;
	while (true)
	{
		while (SDL_PollEvent(&event) > 0)
		{
			if (event.type == SDL_QUIT)	return false;

			if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				SDL_Point tempMouse;
				tempMouse.x = event.button.x;
				tempMouse.y = event.button.y;
				if (SDL_PointInRect(&tempMouse, &rectBack))
				{
					return true;
				}
			}
			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE) return true;
			}
		}
	}
	return true;
}

bool ShowReferenceScreen()
{
	SDL_RenderClear(renderer);
	TTF_Font* font = TTF_OpenFont("Resource\\MTCORSVA.TTF", 32);
	SDL_Color color = { 18, 255, 0 };
	static_assert(sizeof(wchar_t) == sizeof(Uint16));
	SDL_Surface* surface = TTF_RenderUNICODE_Blended(font, reinterpret_cast<Uint16 const *>(L"Автор: Винарчук Александра, студентка группы И503Б"), color);
	SDL_Texture * texInfo = SDL_CreateTextureFromSurface(renderer, surface);
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(texInfo, NULL, NULL, &texW, &texH);
	SDL_Rect rectInfo = { (hResolution - texW) / 2, vResolution / 2 - texH - 50, texW, texH };
	SDL_RenderCopy(renderer, texInfo, NULL, &rectInfo);
	SDL_FreeSurface(surface);

	font = TTF_OpenFont("Resource\\MTCORSVA.TTF", 48);
	surface = TTF_RenderUNICODE_Blended(font, reinterpret_cast<Uint16 const *>(L"Назад"), color);
	SDL_Texture * texBack = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_QueryTexture(texBack, NULL, NULL, &texW, &texH);
	SDL_Rect rectBack = { (hResolution - texW) / 2,  vResolution - 100, texW, texH };
	SDL_RenderCopy(renderer, texBack, NULL, &rectBack);
	SDL_FreeSurface(surface);
	SDL_RenderPresent(renderer);

	SDL_Event event;
	bool isSelected = false;
	while (true)
	{
		while (SDL_PollEvent(&event) > 0)
		{
			if (event.type == SDL_QUIT)	return false;

			if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				SDL_Point tempMouse;
				tempMouse.x = event.button.x;
				tempMouse.y = event.button.y;
				if (SDL_PointInRect(&tempMouse, &rectBack))
				{
					return true;
				}
			}
			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE) return true;
			}
		}
	}
	return true;
}

int ShowMenuItemScreen()
{
	TTF_Font* font = TTF_OpenFont("Resource\\MTCORSVA.TTF", 72);
	SDL_Color color = { 18, 255, 0 };
	static_assert(sizeof(wchar_t) == sizeof(Uint16));
	SDL_Surface* surface = TTF_RenderUNICODE_Blended(font, reinterpret_cast<Uint16 const *>(L"Играть"), color);
	SDL_Texture * texPlay = SDL_CreateTextureFromSurface(renderer, surface);
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(texPlay, NULL, NULL, &texW, &texH);
	SDL_Rect rectPlay = { (hResolution - texW) / 2, vResolution/2 - texH - 100, texW, texH };
	SDL_RenderCopy(renderer, texPlay, NULL, &rectPlay);
	SDL_FreeSurface(surface);

	font = TTF_OpenFont("Resource\\MTCORSVA.TTF", 48);
	surface = TTF_RenderUNICODE_Blended(font, reinterpret_cast<Uint16 const *>(L"Правила игры"), color);
	SDL_Texture * texRule = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_QueryTexture(texRule, NULL, NULL, &texW, &texH);
	SDL_Rect rectRule = { (hResolution - texW) / 2,  vResolution / 2 - texH - 30, texW, texH };
	SDL_RenderCopy(renderer, texRule, NULL, &rectRule);
	SDL_FreeSurface(surface);

	font = TTF_OpenFont("Resource\\MTCORSVA.TTF", 48);
	surface = TTF_RenderUNICODE_Blended(font, reinterpret_cast<Uint16 const *>(L"Справка"), color);
	SDL_Texture * texRef = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_QueryTexture(texRef, NULL, NULL, &texW, &texH);
	SDL_Rect rectRef = { (hResolution - texW) / 2, vResolution / 2 - 10, texW, texH };
	SDL_RenderCopy(renderer, texRef, NULL, &rectRef);
	SDL_FreeSurface(surface);

	font = TTF_OpenFont("Resource\\MTCORSVA.TTF", 48);
	surface = TTF_RenderUNICODE_Blended(font, reinterpret_cast<Uint16 const *>(L"Выход"), color);
	SDL_Texture * texExit = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_QueryTexture(texExit, NULL, NULL, &texW, &texH);
	SDL_Rect rectExit = { (hResolution - texW) / 2,  vResolution - 100, texW, texH };
	SDL_RenderCopy(renderer, texExit, NULL, &rectExit);
	SDL_FreeSurface(surface);

	//отрисовываем надписи
	SDL_RenderPresent(renderer);

	SDL_Event event;
	bool isSelected = false;
	while (true)
	{
		while (SDL_PollEvent(&event) > 0)
		{
			if (event.type == SDL_QUIT)	return 0;

			if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				SDL_Point tempMouse;
				tempMouse.x = event.button.x;
				tempMouse.y = event.button.y;
				if (SDL_PointInRect(&tempMouse, &rectPlay))
				{
					return 1;
				}
				if (SDL_PointInRect(&tempMouse, &rectRule))
				{
					return 2;
				}
				if (SDL_PointInRect(&tempMouse, &rectRef))
				{
					return 3;
				}
				if (SDL_PointInRect(&tempMouse, &rectExit))
				{
					return 0;
				}
			}
			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE) return 0;
			}
		}
	}
	return 0;
}

// 1 - Назад
// 2 - Вперед
// 0 - Закрыть
int ShowMenuScreen()
{
	SDL_RenderClear(renderer);
	int result = ShowMenuItemScreen();
	while (true)
	{
		if (result == 1) return 2;
		if (result == 2)
			if (!ShowRuleScreen())
			{
				return 0;
			}
		if (result == 3)
			if (!ShowReferenceScreen())
			{
				return 0;
			}
		if (result == 0)
		{
			return 0;
		}
		SDL_RenderClear(renderer);
		result = ShowMenuItemScreen();
		if (result == 0) return 0;
	}
}

// 1 - Назад
// 2 - Вперед
// 0 - Закрыть
int ShowSettingScreen(SDL_Point &size)
{
	SDL_RenderClear(renderer);

	TTF_Font* font = TTF_OpenFont("Resource\\MTCORSVA.TTF", 48);
	SDL_Color color = { 18, 255, 0 };
	static_assert(sizeof(wchar_t) == sizeof(Uint16));
	SDL_Surface* surface = TTF_RenderUNICODE_Blended(font, reinterpret_cast<Uint16 const *>(L"Выберите размер"), color);
	SDL_Texture * texChoose = SDL_CreateTextureFromSurface(renderer, surface);
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(texChoose, NULL, NULL, &texW, &texH);
	SDL_Rect dstrect = { (hResolution - texW) / 2, 100, texW, texH };
	SDL_RenderCopy(renderer, texChoose, NULL, &dstrect);

	font = TTF_OpenFont("Resource\\MTCORSVA.TTF", 48);
	surface = TTF_RenderUNICODE_Blended(font, reinterpret_cast<Uint16 const *>(L"Назад"), color);
	SDL_Texture * texBack = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_QueryTexture(texBack, NULL, NULL, &texW, &texH);
	SDL_Rect rectBack = { (hResolution - texW) / 2,  vResolution - 100, texW, texH };
	SDL_RenderCopy(renderer, texBack, NULL, &rectBack);
	SDL_RenderPresent(renderer);

	SDL_Event event;
	bool isSizeSelected = false;
	SDL_Surface* button3x2 = IMG_Load("Resource\\3x2.png");
	if (button3x2 == NULL)
		MessageBox(NULL, IMG_GetError(), "", 0);
	SDL_Texture* texture3x2 = SDL_CreateTextureFromSurface(renderer, button3x2);
	SDL_Surface* button4x3 = IMG_Load("Resource\\4x3.png");
	SDL_Texture* texture4x3 = SDL_CreateTextureFromSurface(renderer, button4x3);
	SDL_Surface* button6x4 = IMG_Load("Resource\\6x4.png");
	SDL_Texture* texture6x4 = SDL_CreateTextureFromSurface(renderer, button6x4);

	int delimeterSize = (hResolution - 300 * 3) / 4;
	SDL_Rect bPos3x2;
	bPos3x2.x = delimeterSize;
	bPos3x2.y = vResolution / 2 - 75;
	bPos3x2.w = 300;
	bPos3x2.h = 150;
	SDL_RenderCopy(renderer, texture3x2, NULL, &bPos3x2);
	SDL_Rect bPos4x3;
	bPos4x3.x = bPos3x2.x + 300 + delimeterSize;
	bPos4x3.y = vResolution / 2 - 75;
	bPos4x3.w = 300;
	bPos4x3.h = 150;
	SDL_RenderCopy(renderer, texture4x3, NULL, &bPos4x3);
	SDL_Rect bPos6x4;
	bPos6x4.x = bPos4x3.x + 300 + delimeterSize;
	bPos6x4.y = vResolution / 2 - 75;
	bPos6x4.w = 300;
	bPos6x4.h = 150;
	SDL_RenderCopy(renderer, texture6x4, NULL, &bPos6x4);
	SDL_RenderPresent(renderer);// произведение отобраения текстур
	SDL_Point result;
	result.x = 1;
	result.y = 1;
	while (!isSizeSelected)
	{
		while (SDL_PollEvent(&event) > 0)
		{
			if (event.type == SDL_QUIT)	return 0;

			if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				SDL_Point tempMouse;
				tempMouse.x = event.button.x;
				tempMouse.y = event.button.y;
				if (SDL_PointInRect(&tempMouse, &bPos3x2))
				{
					result.x = 3;
					result.y = 2;
					isSizeSelected = true;
				}
				if (SDL_PointInRect(&tempMouse, &bPos4x3))
				{
					result.x = 4;
					result.y = 3;
					isSizeSelected = true;
				}
				if (SDL_PointInRect(&tempMouse, &bPos6x4))
				{
					result.x = 6;
					result.y = 4;
					isSizeSelected = true;
				}
				if (SDL_PointInRect(&tempMouse, &rectBack))
				{
					return 1;
				}
			}
		}
	}
	size.x = result.x;
	size.y = result.y;
	return 2;
}

void ReadImagesList()
{
	std::string path = "Images/";
	int i = 0;
	for (const auto & entry : fs::directory_iterator(path))//бежим по всем файлам папки
	{
		std::string path_string{ entry.path().u8string() };
		SDL_Texture * tex = get_texture(renderer, path_string, 150);
		SDL_Rect pos;
		pos.x = (i % 5) * 200 + 25;
		pos.y = (i / 5) * 200 + 25;
		pos.w = 150;
		SDL_Point size;
		SDL_QueryTexture(tex, NULL, NULL, &size.x, &size.y);
		pos.h = size.y * 150 / size.x;
		vImages.push_back(new TextureEx(path_string, pos, tex));
		i++;
		if (i >= 20) break;
	}
}

// 1 - Назад
// 2 - Вперед
// 0 - Закрыть
int ShowImageLibraryScreen(std::string &filename)
{
	ReadImagesList();
	SDL_RenderClear(renderer);
	TTF_Font* font = TTF_OpenFont("Resource\\MTCORSVA.TTF", 48);
	SDL_Color color = { 18, 255, 0 };
	static_assert(sizeof(wchar_t) == sizeof(Uint16));
	SDL_Surface * surface = TTF_RenderUNICODE_Blended(font, reinterpret_cast<Uint16 const *>(L"Назад"), color);
	SDL_Texture * texBack = SDL_CreateTextureFromSurface(renderer, surface);
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(texBack, NULL, NULL, &texW, &texH);
	SDL_Rect rectBack = { (hResolution - texW) / 2,  vResolution - 100, texW, texH };
	SDL_RenderCopy(renderer, texBack, NULL, &rectBack);
	SDL_RenderPresent(renderer);
	for (int i = 0; i < vImages.size(); i++)
	{
		SDL_RenderCopy(renderer, vImages[i]->GetTexture(), NULL, &vImages[i]->GetPosition());
	}
	SDL_RenderPresent(renderer);
	bool isImageSelected = false;
	SDL_Event event;
	while (!isImageSelected)
	{
		while (SDL_PollEvent(&event) > 0)
		{
			if (event.type == SDL_QUIT)	return 0;

			if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				SDL_Point tempMouse;
				tempMouse.x = event.button.x;
				tempMouse.y = event.button.y;
				for (int i = 0; i < vImages.size(); i++)
				{
					if (SDL_PointInRect(&tempMouse, &vImages[i]->GetPosition()))
					{
						filename = vImages[i]->GetPath();
						isImageSelected = true;
					}
				}
				if (SDL_PointInRect(&tempMouse, &rectBack))
				{
					return 1;
				}
			}
		}
	}
	return 2;
}

// Реализация отображения игрового процесса
// Параметры:
//    size - Размер сетки
//    filename - имя файла картинки
//    vSprite - хранилище фрагментов паззла
// Возвращаемое значение:
//    0 - Закрыть программу
//    1 - Переход к предыдущему экрану
//    2 - Переход к следующему экрану
int ShowPlayScreen(SDL_Point &size, std::string &filename, std::vector<Piece*> &vSprite)
{
	bool isRun = true;
	SDL_Event event;
	bool isPiecePressed = false;
	SDL_Cursor* cursor;

	hCount = size.x;
	vCount = size.y;

	texture = get_texture(renderer, filename, (hResolution / (hCount + 2))*hCount);

	SDL_Point pole;
	pole.x = hResolution;
	pole.y = vResolution;
	RecalcPole(pole, vSprite); //формирование нового паззла
	SDL_RenderClear(renderer);
	DrawPieces(vSprite);
	SDL_RenderPresent(renderer);

	while (isRun)
	{
		while (SDL_PollEvent(&event) > 0)
		{
			if (event.type == SDL_QUIT) return 0;

			if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				SDL_RenderClear(renderer);

				bool flag = false;
				for (int i = hCount * vCount - 1; i >= 0; i--)
				{
					SDL_Rect spritePos = (*vSprite[i]).GetPieceDestination();
					SDL_Point tempMouse;
					tempMouse.x = event.button.x;
					tempMouse.y = event.button.y;
					if (SDL_PointInRect(&tempMouse, &spritePos))
					{
						if (!(*vSprite[i]).GetPieceIsPlaced())
						{
							//offsetMouse = tempMouse - spritePos;
							isPiecePressed = true;
							cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
							SDL_SetCursor(cursor);
							Piece sprite = *vSprite[i];
							vSprite.erase(vSprite.begin() + i);
							vSprite.push_back(new Piece(sprite));
							break;
						}
					}
				}

				DrawPieces(vSprite);
				SDL_RenderPresent(renderer);
			}
			if (event.type == SDL_MOUSEBUTTONUP)
			{
				Piece sprite = *vSprite[hCount*vCount - 1];
				SDL_Rect newPos = sprite.GetPieceDestination();
				if (abs((sprite.GetPieceNumber() % hCount)*pSize + hBackOffset - newPos.x) < 20 and abs((sprite.GetPieceNumber() / hCount)*pSize + vBackOffset - newPos.y) < 20)
				{
					newPos.x = (sprite.GetPieceNumber() % hCount)*pSize + hBackOffset;
					newPos.y = (sprite.GetPieceNumber() / hCount)*pSize + vBackOffset;
					sprite.SetPieceDestination(newPos);
					vSprite[hCount*vCount - 1]->SetPieceDestination(newPos);
					vSprite[hCount*vCount - 1]->SetPieceIsPlaced(true);
				}
				cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
				SDL_SetCursor(cursor);
				isPiecePressed = false;
				SDL_RenderClear(renderer);
				DrawPieces(vSprite);
				SDL_RenderPresent(renderer);
			}
			if (event.type == SDL_MOUSEMOTION)
			{
				if (isPiecePressed)
				{
					SDL_RenderClear(renderer);
					DrawPieces(vSprite);
					SDL_Point mouseCoord;
					mouseCoord.x = event.button.x;
					mouseCoord.y = event.button.y;
					Piece sprite = *vSprite[hCount*vCount - 1];
					SDL_Rect spritePos = sprite.GetPieceDestination();
					if (SDL_PointInRect(&mouseCoord, &spritePos))
					{
						SDL_Rect newPos = sprite.GetPieceDestination();
						newPos.x += event.motion.xrel;
						newPos.y += event.motion.yrel;
						sprite.SetPieceDestination(newPos.x, newPos.y, newPos.w, newPos.h);
						(vSprite[hCount*vCount - 1])->SetPieceDestination(newPos.x, newPos.y, newPos.w, newPos.h);
						SDL_RenderCopy(renderer, texture, &sprite.GetPieceSource(), &sprite.GetPieceDestination());
					}
					SDL_RenderPresent(renderer);
				}
			}
		}
		if (IsComplete(vSprite)) return 2;
	}
	return 0;
}

int main(int argc, char ** argv)
{
	setlocale(LC_CTYPE, "rus");
	std::srand(static_cast<unsigned int>(time(0)));

	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	window = SDL_CreateWindow("Puzzle", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, hResolution, vResolution, 0);
	SDL_Surface* sIcon = IMG_Load("Resource\\icon.png");
	SDL_SetWindowIcon(window, sIcon);
	SDL_FreeSurface(sIcon);
	renderer = SDL_CreateRenderer(window, -1, 0);
	//SDL_SetRenderDrawColor(renderer, 94, 151, 224, 1);
	int CurrentScreen = 0;
	ShowStartScreen();
	CurrentScreen++;
	//return 0;
	SDL_Point size;
	std::string filename;
	bool isRun = true;
	while (isRun)
	{
		int result = -1;
		switch (CurrentScreen)
		{
			case 1:
				result = ShowMenuScreen();
				if (result == 0) isRun = false;
				if (result == 2) CurrentScreen++;
				break;
			case 2:
				result = ShowSettingScreen(size);
				if (result == 0) isRun = false;
				if (result == 1) CurrentScreen--;
				if (result == 2) CurrentScreen++;
				break;
			case 3:
				result = ShowImageLibraryScreen(filename);
				if (result == 0) isRun = false;
				if (result == 1) CurrentScreen--;
				if (result == 2) CurrentScreen++;
				break;
			case 4:
				vSprite.clear();//очистка пазла, хранилища фрагментов
				vImages.clear();
				result = ShowPlayScreen(size, filename, vSprite);
				if (result == 0) isRun = false;
				if (result == 1) CurrentScreen--; 
				if (result == 2) CurrentScreen++;
				break;
			case 5:
				result = ShowCompleteScreen();
				if (result == 0) isRun = false;
				if (result == 1) CurrentScreen--;
				if (result == 2) CurrentScreen = 1; 
				break;
		default:
			break;
		}
	}

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();

	return 0;
}