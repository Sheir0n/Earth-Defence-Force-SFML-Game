#pragma once
#include "Unit.h"
#include "Weapon.h"
#include "AudioPlayer.h"
#include "RocketBullet.h"
#include <algorithm> 
#include <chrono>
#include <random>
#include <unordered_map>
#include <iostream>

#include <SFML/Audio.hpp>

class Player : public Unit {
private:
	enum AnimStates { Entrance, Idle, Down, Up, Dying }animState;
	sf::Vector2<int> directionVector;

	//acceleration
	sf::Vector2f acc;
	const float accLimit = 1.65f;
	const float speed = 8.35f;

	void CalculateAcceleration(float _deltaTime);
	void UpdatePosition(float _deltaTime);
	sf::Vector2<int> GetDirectionInput();

	//input mamnagement
	//input is recived from game class passed by gameplay manager
	//will not execute if block is true
	bool blockControls = true;
	bool dodge = false;
	bool shootLeft = false;
	bool shootRight = false;

	float dodgeCooldownTimer = 2000;

	const float dodgeCooldownInMs = 1000;
	const float dodgeDurationInMs = 225;
	const float dodgeStrength = 2.65f;
	//tells if player is mid dodge
	bool dodgeMotion = false;
	//used to set direction for dodge
	sf::Vector2<int> dodgeVector;


	//weapon and dodge inputs
	//if dodge true and not on cooldown makes dodge motion
	void ExecuteDodgeInput(float _deltaTime);
	//if weapon ready fires weapon
	void FireWeapons();
	void AutomaticWeaponsFireInput();

	//shield
	int shield;
	int maxShield;

	const float shieldRegenTimeMs = 5000;
	const float shieldRegenTickTimeMs = 200;
	bool shieldRegenSound = false;

	float shieldRegenCooldownTimer = 0;
	float shieldRegenTickTimer = 0;

	void RegenShield(float _deltaTime);

	//load sounds
	void LoadSounds(const std::unordered_map<SoundId, sf::SoundBuffer>& _playerSounds);

	//weapons
	std::pair<Weapon*, Weapon*> weapons;

	std::pair<Bullet*, Bullet*> newBullets;
	//returns new bullet depending on weapontype in pointer
	Bullet* CreateNewBullet(const Weapon*);

	bool readyForAction = false;
	float deathEffectTimer;
	const float deathTimeMs = 4907;

public:
	enum PlayerState { Entry, Logic, Destroyed, Gone }logicState = Entry;

	Player() = delete;
	Player(const std::unordered_map<SoundId, sf::SoundBuffer>& _playerSounds, const sf::Texture* _texture, std::pair<Weapon::WeaponType, Weapon::WeaponType>);
	~Player();
	void OnUpdate(float _deltaTime);

	//gets input data from gameplay manager
	//used for one time button presses
	void ActionInput(int _inputId);

	bool CheckIfDodge() const;
	sf::Vector2<int> GetDodgeVector() const;

	//used to return bool for highlighting dodge cooldown
	bool CheckDodgeReadyHighlight() const;
	bool CheckShield() const;
	void Animate();

	//return player data used in ui
	//shield, hp, maxAmmo1, ammo1, maxAmmo2, ammo2
	std::tuple<float, float, int, int, int, int> GetData() const;

	std::pair<Bullet*, Bullet*> GetNewBullets();

	void TakeDamage(int _dmgValue, bool _useTimer);

	bool IsEntryOver() const;

	PlayerState GetState() const;

	void FullRegenShield();

	std::pair<int, int> GetWeapons() const;
};