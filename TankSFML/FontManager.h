#include <SFML\Graphics.hpp>

//----------------------------------------------------------------

enum Font

{

	FontArial,
	FontVAGWorld,

	FontAmount

};

//----------------------------------------------------------------

struct FontResource

{

	int number_;

	const char* path_;

};

//----------------------------------------------------------------

struct FontManager

{

	sf::Font fonts_[FontAmount];

	const char* path_;

	FontManager (FontResource * recources, const char* path);

	sf::Font getFont (int number);

};

//----------------------------------------------------------------

FontManager::FontManager (FontResource * resources, const char* path) :

	fonts_ (),
	path_  (path)

{

	for (int i = 0; i < FontAmount; i++)

	{

		char path[100] = "";

		sprintf_s (path, "%s\\%s", path_, resources[i].path_);

		sf::Font font;
		font.loadFromFile (path);

		fonts_[i] = font;

	}

}

//----------------------------------------------------------------

sf::Font FontManager::getFont (int number)

{

	return fonts_[number];

}

//----------------------------------------------------------------

FontResource Fonts[] = {

	{FontArial,       "Arial.ttf"},
	{FontVAGWorld, "VAGWorld.otf"}

};