#pragma once

#include <SFML/Audio.hpp>

#include <iostream>
#include <chrono>
#include <random>

#include "AudioPlayer.h"
#include "Bullet.h"
#include "Enums.h"

#include "PlayerPrefs.h"
class Weapon {
private:
	int maxAmmo;
	int ammo;
	float reloadTimeInMs;
	//decides is mouse can be held down to fire
	bool isAutomatic;

	bool isReloading = false;

	float timeBetweenShotsInMs;

	float reloadTimer = 0;
	float fireTimer = 0;
	SoundId weaponFireSound;

	AudioPlayer* audioPlayer;

	//if weapon is reloading checks time to change status
	void UpdateReloadStatus();

	//weapon startup
	float startFireSlowdown = 0;
	float startFireSlowdownReduction = 100;
	bool firingPressed = false;
public:
	enum WeaponType { Blaster, Spitter, RailPiercer, FoeSeeker  }type;
	
	Weapon(WeaponType _type, const std::unordered_map<SoundId, sf::SoundBuffer>& _playerSounds);
	~Weapon();

	bool IsReady();
	Bullet* Fire();
	bool IsAutomatic() const;

	//returns ammo count
	std::pair<int, int> GetAmmoData();

	//after player presses R+mouse forces weapon to reload
	void ForceReload();

	WeaponType GetWeaponType() const;

	void OnUpdate(float _deltaTime);
};
