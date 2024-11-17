#pragma once
#include "bass.h"
#pragma comment(lib, "bass.lib")

enum class Audio
{
	NONE,
	MUS_EXIT,
	MUS_OVER,
	MUS_STAGE_1,
	MUS_STAGE_2,
	MUS_STAGE_3,
	MUS_STAGE_4,
	MUS_STAGE_5,

	SND_INIT,
	SND_COLLECT,
	SND_ENTER,
	SND_MOVE,

	AMB_NEAR,
	AMB_PROX
};


class AudioPlayer
{
	HSTREAM hstream_background;
	HSTREAM hstream_user;
	HSTREAM hstream_ambient;
	Audio ambient_last;
public:
	bool mute;

	AudioPlayer();

	void PlayBGM(Audio audio);
	void PlayUserSFX(Audio audio, const int& variation);
	void PlayAmbientSFX(Audio audio);
};