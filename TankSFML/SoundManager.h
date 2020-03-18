#pragma once

#include <stdio.h>
#include <SFML\Audio.hpp>
#include "C:\Lib\ExtraUtilits\ExtraUtilits.h"

//----------------------------------------------------------------

enum Sound

{

	SoundError,
	SoundItemTake,
	SoundItemSpawn,
	SoundCoin,

	SoundAmount

};

//----------------------------------------------------------------

struct SoundResource

{

	int number_;

	const char* path_;

	SoundResource (int number, const char* path);

};

SoundResource::SoundResource (int number, const char* path) :

	number_ (number),
	path_   (path)

{}

//----------------------------------------------------------------

struct SoundPlayer

{

	sf::SoundBuffer buffer_;
	sf::Sound sound_;

	SoundPlayer ();

	bool load (const char* path);

	void play ();

};

SoundPlayer::SoundPlayer () :

	buffer_ (),
	sound_  ()

{}

//----------------------------------------------------------------

struct SoundManager

{

	FILE* logFile;

	SoundPlayer* sounds_[SoundAmount];

	SoundResource* resources_;

	SoundPlayer* errorSound_;

	SoundManager (SoundResource* resources, const char* resourcesPath);

	~SoundManager ();

	SoundPlayer* getSound (int sound);

};

SoundManager::SoundManager (SoundResource* resources, const char* resourcesPath) :

	sounds_     (),
	resources_  (resources),
	errorSound_ (nullptr)

{

	fopen_s (&logFile, "SoundManagerLog.txt", "w");

	fprintf (logFile, "Sound manager started. Loading sounds...\n");

	errorSound_ = new SoundPlayer ();
	errorSound_->load ("Resources\\Sound\\Error.wav");

	for (int i = 0; i < SoundAmount; i++)

	{

		char path[256] = "";

		sprintf_s (path, "%s\\%s", resourcesPath, resources[i].path_);

		fprintf (logFile, "Loading sound #%d, %s...\n", i, path);

		sounds_[i] = new SoundPlayer ();

		if (sounds_[i]->load (path)) fprintf (logFile, "Succesful\n");

		else

		{

			fprintf (logFile, "Failed: file \"%s\" does not exists.\n", path);

			sprintf_s (path, "%s\\%s", resourcesPath, "Error.wav");

			sounds_[i]->load (path);

		}

	}

}

SoundManager::~SoundManager ()

{

	for (int n = 0; n < SoundAmount; n++)

	{

		if (!sounds_[n]) continue;

		delete (sounds_[n]);

		sounds_[n] = nullptr;

	}

	fprintf (logFile, "Closing log file...");

	fclose (logFile);

}

//----------------------------------------------------------------

bool SoundPlayer::load (const char* path)

{

	if (eu::fileExists (path))

	{

		buffer_.loadFromFile (path);
		sound_.setBuffer (buffer_);

		return true;

	}

	else

	{

		return false;

	}

}

//----------------------------------------------------------------

void SoundPlayer::play ()

{

	sound_.play ();

}

//----------------------------------------------------------------

SoundPlayer* SoundManager::getSound (int sound)

{

	fprintf (logFile, "%s: Passing sound #%d...\n", __func__, sound);

	if (sound < 0 || sound >= SoundAmount)

	{

		fprintf (logFile, "Failed: wrong sound number\n");

		return errorSound_;

	}

	fprintf (logFile, "succesful\n");

	return sounds_[sound];

}

//----------------------------------------------------------------

SoundResource Sounds[] =

{

	{SoundError,     "Error.wav"    },
	{SoundItemTake,  "ItemTake.wav" },
	{SoundItemSpawn, "ItemSpawn.wav"},
	{SoundCoin,      "Coin.wav"     }

};