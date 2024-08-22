#pragma once
#include "SoLoud/include/soloud.h"
#include "SoLoud/include/soloud_wav.h"
#include <string>
#include "SoLoud/include/soloud_wavstream.h"

class SoundPlayer
{
public:
	SoundPlayer();
	~SoundPlayer();
	SoundPlayer(const SoundPlayer&) = delete;
	SoundPlayer& operator=(const SoundPlayer&) = delete;

	void LoadSong(const std::string& fileName);
	void Play(const bool loop) noexcept;
	double GetCurrentPlaytime() noexcept;
private:
	SoLoud::Soloud engine;
	SoLoud::Wav mainSong;
	SoLoud::handle mainSongHandle;
};
