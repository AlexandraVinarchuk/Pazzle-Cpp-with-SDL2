# Паззлы

## Описание основных переменных, определяющих размер

Программа называется Puzzle. Разработка программы начинается с описания используемых входных данных, в частности, задается размер окна, который хранится в переменных hResolution и wResolution.
Далее задаем начальное количество клеток hCount и wCount и размер стороны квадрата клетки pSize. Определяем размер отступа:
```
int hBackOffset = (hResolution - hCount * pSize) / 2;
int vBackOffset = (vResolution - vCount * pSize) / 2;
```
## Описание функций
-	int getRandomNumber(int min, int max) – распределяет случайного числа в заданном диаапазоне;
-	bool IsComplete(std::vector<Piece*> &vSprite) – проверяет поставлен ли фрагмент паззла на место;
-	int ShowCompleteScreen() – показывает экран с собранной картинкой и кнопкой Continue, которая возвращает нас в главное меню;
-	void DrawPieces(std::vector<Piece*> &vSprite) – отрисовывает сначала сетку, потом установленные правильно фрагменты, потом неустановленные фрагменты. Именно в таком порядке, чтобы сетка была в самом низу, потом шли установленные и не перекрывали неустановленные;
-	SDL_Texture*get_texture(SDL_Renderer*pRenderer,std::string image_filename, int limit) – формирование текстуры, указанного размера, из изображения, находящегося по указанному пути;
-	void RecalcPole(SDL_Point PoleResolution, std::vector<Piece*> &vSprite) – пересчет игрового поля при начале новой игры, случайным образом раскидывает фрагменты;
-	void ShowStartScreen() – показывает начальный экран;
-	bool ShowRuleScreen() – показывает экран с правилами игры;
-	bool ShowReferenceScreen() – показывает экран со справкой, в которой указан автор;
-	int ShowMenuItemScreen() – отрисовывает надписи в меню и обрабатывает события;
-	int ShowMenuScreen() – обрабатывает выбор;
-	int ShowSettingScreen(SDL_Point &size) – показывает экран с выбором размера паззла и обрабатывает выбор;
-	void ReadImagesList() – открывает файл с загруженными заранее картинками;
-	int ShowImageLibraryScreen(std::string &filename) – показывает экран с картинками и обрабатывает выбор;
-	int ShowPlayScreen(SDL_Point &size,std::string &filename, std::vector<Piece*> &vSprite) – реализация отображения игрового процесса.

## Демоснтрация работы
![image](https://github.com/user-attachments/assets/77168253-17a6-465a-a239-ba05ed5de807)

![image](https://github.com/user-attachments/assets/59986a2e-be9e-440a-9c41-63562d266d2d)

![image](https://github.com/user-attachments/assets/cbff96e4-84f0-476f-8dcf-8390eaf10ae6)

![image](https://github.com/user-attachments/assets/b9cd8a53-e129-4b38-9cae-2b4470d87370)

![image](https://github.com/user-attachments/assets/0e9ea0f9-0214-498b-9037-f7a3f5db143c)

![image](https://github.com/user-attachments/assets/d578afe8-56ac-44c5-bdd0-572363ab3039)
