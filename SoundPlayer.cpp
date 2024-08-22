#include "SoundPlayer.h"
#include <stdexcept>

SoundPlayer::SoundPlayer()
{
	engine.init();
}

SoundPlayer::~SoundPlayer()
{
	engine.deinit();
}

void SoundPlayer::LoadSong(const std::string& fileName)
{
	const SoLoud::result returnCode = mainSong.load(fileName.c_str());
	if (returnCode > 0)
	{
		throw std::runtime_error(engine.getErrorString(returnCode));
	}
}

void SoundPlayer::Play(const bool loop) noexcept
{
	mainSong.setLooping(loop);
	mainSongHandle = engine.play(mainSong);
}

double SoundPlayer::GetCurrentPlaytime() noexcept
{
	return engine.getStreamTime(mainSongHandle);
}