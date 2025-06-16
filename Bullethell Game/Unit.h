#pragma once

#include "GameObject.h"
#include "AudioPlayer.h"

#include <iostream>
#include <chrono>
#include <unordered_map>
#include <thread>

#include "PlayerPrefs.h"
class Unit : public GameObject {
protected:
	//universal unit variables
	int maxHealth = 10;
	int health = 10;
	int id = 0;
	int contactDamage = 0;

	//sprite and animation data, <int,int> - animation sprite offset ranges
	sf::Sprite sprite;
	int animationFrame = 1;

	//position of center of the object
	sf::Vector2f fixedPosition;

	sf::Vector2<int> hitbox;
	//hitbox offset from position
	sf::Vector2<int> hitboxOffset;
	//position of center of the hitbox 
	sf::Vector2f fixedHitboxPos;

	bool hitboxActive = true;

	//animation
	std::unordered_map<int, std::pair<int, int>> animationData;

	//sounds
	AudioPlayer* audioPlayer = nullptr;
	virtual void LoadSounds(const std::unordered_map<SoundId, sf::SoundBuffer>& _sounds) = 0;

	//used in calculating direction
	int GetSign(float _value);

	void UpdatePosition(float _deltaTime);

	//damage
	float damageInvTimer = 0;
	float damageHighlightTimer = 0;
	int damageInvMs = 200;

	bool isDying = false;

	bool DamageTimerCheck();

	void UpdateUnitTimers(float _deltaTime);
public:
	Unit(int _id, int _width, int _height, sf::Vector2f _position, const sf::Texture* _texture, sf::Vector2<int> _hitbox, sf::Vector2<int> _hitboxOffset);
	//sets hitbox size
	void SetHitbox(sf::Vector2<int> _hitbox);
	//activates or disactivates hitbox collision check
	void SetHitboxActive(bool _isActive);

	virtual void TakeDamage(int _value, bool _useTimer) = 0;

	const sf::Sprite& GetDrawData();

	virtual void Animate() = 0;

	sf::Vector2f GetFixedPos() const;

	bool CheckIfHit(const sf::Vector2f _hitboxPos, const sf::Vector2i _hitbox);

	bool IsHitboxActive() const;

	int GetId() const;

	bool CheckDamagedHighlight() const;

	//returns pos of center of hitbox
	sf::Vector2f GetFixedHitbox() const;
	//returns hitbox dimensions
	sf::Vector2i GetHitbox() const;

	int GetContactDamage() const;
};