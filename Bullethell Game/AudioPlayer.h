#pragma once
#include <SFML/Audio.hpp>
#include <vector>
#include <random>
#include <unordered_map>
#include <iostream>

#include "Enums.h"

class AudioPlayer {
private:
	std::unordered_map<SoundId, sf::Sound> sounds;
	std::unordered_map<SoundId, int> soundVolumeData;
	std::unordered_map<SoundId, float> soundPitchData;
public:
	AudioPlayer() {};
	~AudioPlayer();

	void LoadSound(SoundId _id, const sf::SoundBuffer* _buffer, int _volume, float _pitch);
	void PlaySoundId(SoundId _id, float _volumeModifier);
	void PlaySoundId(SoundId _id, float _volumeModifier, float _pitchFluctuation);
	void PlaySoundId(std::vector<SoundId>& _ids, float _volumeModifier, float _pitchFluctuation);

	bool PlayIfFinished(SoundId _id, float _volumeModifier, SoundId _check);
	void SetLoop(SoundId _id, bool _value);

	void StopSound(SoundId _id);
	int GetSoundLength(SoundId _id);

	void SetVolume(SoundId _id, int _value);
};