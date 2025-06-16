#include "AudioPlayer.h"


//required for device closing error
AudioPlayer::~AudioPlayer() {
	for (auto& it : sounds) {
		StopSound(it.first);
	}
};

void AudioPlayer::LoadSound(SoundId _id, const sf::SoundBuffer* _buffer, int _volume, float _pitch) {
	sf::Sound _sound;
	_sound.setBuffer(*_buffer);
	_sound.setVolume((float)_volume);
	_sound.setPitch(_pitch);

	this->sounds.insert(std::make_pair(_id, std::move(_sound)));
	this->soundVolumeData.insert(std::make_pair(_id, _volume));
	this->soundPitchData.insert(std::make_pair(_id, _pitch));
}

void AudioPlayer::PlaySoundId(SoundId _id, float _volumeModifier)
{
	if (sounds.count(_id)) {
		this->SetVolume(_id, (int)soundVolumeData.at(_id) * _volumeModifier);
		sounds.at(_id).play();
	}
}

int AudioPlayer::GetSoundLength(SoundId _id)
{
	if (sounds.count(_id)) {
		return this->sounds.at(_id).getBuffer()->getDuration().asMilliseconds();
	}
	else
		return 0;
}

void AudioPlayer::SetVolume(SoundId _id, int _value)
{
	this->sounds[_id].setVolume(_value);
}

void AudioPlayer::PlaySoundId(SoundId _id, float _volumeModifier, float _pitchFluctuation)
{
	if (sounds.count(_id) > 0) {
		float _defPitch = soundPitchData.at(_id);

		int _pitchValue = (int)(_pitchFluctuation * 100 * _defPitch);

		std::random_device _dev;
		std::mt19937 _rng(_dev());
		std::uniform_int_distribution<std::mt19937::result_type> _randPitch(0, _pitchValue);

		this->sounds.at(_id).setPitch(_defPitch - (0.5f * (float)_pitchValue * 0.01f) + (float)_randPitch(_rng) * 0.01f);

		this->SetVolume(_id, soundVolumeData.at(_id) * _volumeModifier);
		this->sounds.at(_id).play();
	}
}

void AudioPlayer::PlaySoundId(std::vector<SoundId>& _ids, float _volumeModifier, float _pitchFluctuation)
{
	if (_ids.size() > 0)
	{
		std::random_device _dev;
		std::mt19937 _rng(_dev());
		std::uniform_int_distribution<std::mt19937::result_type> _dist(0, _ids.size() - 1);

		int _index = _dist(_rng);
		float _defPitch = soundPitchData.at(_ids[_index]);
		int _pitchValue = (int)(_pitchFluctuation * 100 * _defPitch);

		std::uniform_int_distribution<std::mt19937::result_type> _randPitch(0, _pitchValue);

		this->sounds.at(_ids[_index]).setPitch(_defPitch - (0.5f * (float)_pitchValue * 0.01f) + (float)_randPitch(_rng) * 0.01f);;

		this->SetVolume(_ids[_index], soundVolumeData.at(_ids[_index]) * _volumeModifier);
		this->sounds.at(_ids[_index]).play();
	}
}

bool AudioPlayer::PlayIfFinished(SoundId _id, float _volumeModifier, SoundId _check)
{
	;
	if (this->sounds.at(_check).getStatus() == 0) {
		this->SetVolume(_id, soundVolumeData.at(_id) * _volumeModifier);
		this->sounds.at(_id).play();
		return true;
	}
	return false;
}

void AudioPlayer::SetLoop(SoundId _id, bool _value)
{
	this->sounds.at(_id).setLoop(_value);
}

void AudioPlayer::StopSound(SoundId _id) {
	if (sounds.count(_id) > 0) {
		sounds.at(_id).stop();
	}
}

