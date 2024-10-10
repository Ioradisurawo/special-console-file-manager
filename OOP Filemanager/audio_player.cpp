#pragma once
#include "audio_player.h"

AudioPlayer::AudioPlayer()
		: mute(false)
		, ambient_last(Audio::NONE)
	{
		BASS_Init(-1, 44100, 0, 0, NULL);
	}

void AudioPlayer::PlayBGM(Audio audio)
	{
		BASS_ChannelFree(hstream_background);

		if (mute)
			return;

		switch (audio)
		{
		case Audio::MUS_EXIT:
		{
			hstream_background = BASS_StreamCreateFile(FALSE, L"data/unknown.wav", 0, 0, BASS_MUSIC_LOOP); //загрузка файла в поток
			BASS_ChannelPlay(hstream_background, true); // проигрывание файла
			break;
		}
		case Audio::MUS_OVER:
		{
			hstream_background = BASS_StreamCreateFile(FALSE, L"data/noise.wav", 0, 0, BASS_MUSIC_LOOP); //загрузка файла в поток
			BASS_ChannelPlay(hstream_background, true); // проигрывание файла
			break;
		}
		case Audio::MUS_STAGE_1:
		{
			hstream_background = BASS_StreamCreateFile(FALSE, L"data/1.wav", 0, 0, BASS_MUSIC_LOOP); //загрузка файла в поток
			BASS_ChannelPlay(hstream_background, true); // проигрывание файла
			break;
		}
		case Audio::MUS_STAGE_2:
		{
			hstream_background = BASS_StreamCreateFile(FALSE, L"data/2.wav", 0, 0, BASS_MUSIC_LOOP);
			BASS_ChannelPlay(hstream_background, true);
			break;
		}
		case Audio::MUS_STAGE_3:
		{
			hstream_background = BASS_StreamCreateFile(FALSE, L"data/3.wav", 0, 0, BASS_MUSIC_LOOP);
			BASS_ChannelPlay(hstream_background, true);
			break;
		}
		case Audio::MUS_STAGE_4:
		{
			hstream_background = BASS_StreamCreateFile(FALSE, L"data/4.wav", 0, 0, BASS_MUSIC_LOOP);
			BASS_ChannelPlay(hstream_background, true);
			break;
		}
		case Audio::MUS_STAGE_5:
		{
			hstream_background = BASS_StreamCreateFile(FALSE, L"data/fun_is_infinite.wav", 0, 0, BASS_MUSIC_LOOP); //загрузка файла в поток
			BASS_ChannelPlay(hstream_background, true); // проигрывание файла
			break;
		}
		default:
			break;
		}
	}
void AudioPlayer::PlayUserSFX(Audio audio, const int& variation)
	{
		BASS_ChannelFree(hstream_user);

		if (mute)
			return;

		switch (audio)
		{
			case Audio::SND_INIT:
			{
				hstream_user = BASS_StreamCreateFile(FALSE, L"data/fun_is_infinite_start.wav", 0, 0, 0);
				BASS_ChannelPlay(hstream_user, true);
				break;
			}
			case Audio::SND_COLLECT:
			{
				hstream_user = BASS_StreamCreateFile(FALSE, L"data/c.wav", 0, 0, 0);
				BASS_ChannelPlay(hstream_user, true);
				break;
			}
			case Audio::SND_ENTER:
			{
				switch (variation)
				{
				case 0:
				{
					hstream_user = BASS_StreamCreateFile(FALSE, L"data/door1.wav", 0, 0, 0);
					break;
				}
				case 1:
				{
					hstream_user = BASS_StreamCreateFile(FALSE, L"data/door2.wav", 0, 0, 0);
					break;
				}
				}
				BASS_ChannelPlay(hstream_user, true);
				break;
			}
			case Audio::SND_MOVE:
			{
				switch (variation)
				{
				case 0:
				{
					hstream_user = BASS_StreamCreateFile(FALSE, L"data/step_a.wav", 0, 0, 0);
					break;
				}
				case 1:
				{
					hstream_user = BASS_StreamCreateFile(FALSE, L"data/step_b.wav", 0, 0, 0);
					break;
				}
				}
				BASS_ChannelPlay(hstream_user, true);
				break;
			}
			default:
				break;
		}
	}
void AudioPlayer::PlayAmbientSFX(Audio audio)
	{
		// Не прерывать если уже играет подобное
		if (audio == ambient_last)
			return;

		BASS_ChannelFree(hstream_ambient);

		if (mute)
			return;

		switch (audio)
		{
		case Audio::AMB_NEAR:
		{
			hstream_ambient = BASS_StreamCreateFile(FALSE, L"data/nearby.wav", 0, 0, BASS_MUSIC_LOOP);
			BASS_ChannelPlay(hstream_ambient, true);
			break;
		}
		case Audio::AMB_PROX:
		{
			hstream_ambient = BASS_StreamCreateFile(FALSE, L"data/proximity.wav", 0, 0, BASS_MUSIC_LOOP);
			BASS_ChannelPlay(hstream_ambient, true);
			break;
		}
		default:
			break;
		}
		ambient_last = audio;
	}
