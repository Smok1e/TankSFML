#include <stdio.h>
#include <SFML\Graphics.hpp>

//----------------------------------------------------------------

const int PATH_MAX = 256;

//----------------------------------------------------------------

enum Image

{

	ImageTank,
	ImageTankBody,
	ImageExplosion,
	ImageEnemy,
	ImageCoin,
	ImageLevelUpText,
	ImageFood,
	ImageIcon,
	ImageMedkit,
	ImageCursor,

	ImageAmount

};

//----------------------------------------------------------------

struct Resource

{

	int number_;

	const char* path_;

	int frames_;

	bool smooth_;

	Resource (int number, const char* path, int frames, bool smooth = false);

};

Resource::Resource (int number, const char* path, int frames, bool smooth) :

	number_ (number),
	path_   (path),
	frames_ (frames),
	smooth_ (smooth)

{}

//----------------------------------------------------------------

struct ResourceManager

{

	FILE * logFile;

	sf::Image images_[ImageAmount];

	Resource* resources_;

	sf::Image errorImage_;

	ResourceManager (Resource * resources, const char * resourcesPath);

	~ResourceManager ();

	sf::Image getImage (int image);

	int getFrames (int image);

	bool getSmooth (int image);

};

ResourceManager::ResourceManager (Resource* resources, const char* resourcesPath) :

	images_     (),
	errorImage_ (),
	resources_  (resources)

{

	errorImage_.loadFromFile ("Resources\\Images\\Error.png");

	fopen_s (&logFile, "ResourceManagerLog.txt", "w");

	fprintf (logFile, "Resource manager started. Loading images...\n");

	for (int i = 0; i < ImageAmount; i++)

	{

		char path [PATH_MAX] = "";

		sprintf_s (path, "%s\\%s", resourcesPath, resources[i].path_);

		fprintf (logFile, "Loading image #%d, %s...\n", i, path);

		sf::Image image;

		if (!image.loadFromFile (path))

		{

			fprintf (logFile, "Failed\n");
			
			image = errorImage_;

		}

		else fprintf (logFile, "Succesful\n");

		images_[i] = image;

	}

}

ResourceManager::~ResourceManager ()

{

	for (int i = 0; i < ImageAmount; i++)

	{

		images_[i].~Image ();

	}

	fprintf (logFile, "Closing log file...");

	fclose (logFile);

}

//----------------------------------------------------------------

Resource Resources[] = {

//	Image number	   Image path        Frames Smooth

	{ImageTank,        "Tank.png",        1         },
	{ImageTankBody,    "TankBody.png",    1         },
	{ImageExplosion,   "Explosion.bmp",   7,    true},
	{ImageEnemy,       "Enemy.bmp",      72         },
	{ImageCoin,        "Coin.bmp",        6         },
	{ImageLevelUpText, "LevelUpText.bmp", 1         },
	{ImageFood,        "Food.bmp",        1,    true},
	{ImageIcon,        "Icon.png",        1,        },
    {ImageMedkit,      "Medkit.bmp",      1         },
	{ImageCursor,      "Cursor.png",      1         }
	
};

//----------------------------------------------------------------

sf::Image ResourceManager::getImage (int image)

{

	fprintf (logFile, "%s: passing image #%d...\n", __func__, image);

	if (image < 0 || image >= ImageAmount)

	{

		fprintf (logFile, "failed: wrong image number\n");

		return errorImage_;

	}

	fprintf (logFile, "succesful\n");

	return images_ [image];

}

//----------------------------------------------------------------

int ResourceManager::getFrames (int image)

{

	fprintf (logFile, "%s: passing frames of image #%d...\n", __func__, image);

	if (image < 0 || image >= ImageAmount)

	{

		fprintf (logFile, "failed: wrong image number\n");

		return 1;

	}

	fprintf (logFile, "succesful\n");

	return resources_[image].frames_;

}

bool ResourceManager::getSmooth (int image)

{

	fprintf (logFile, "%s: passing smooth of image #%d...\n", __func__, image);

	if (image < 0 || image >= ImageAmount)

	{

		fprintf (logFile, "failed: wrong image number\n");

		return false;

	}

	fprintf (logFile, "succesful\n");

	return resources_[image].smooth_;

}
