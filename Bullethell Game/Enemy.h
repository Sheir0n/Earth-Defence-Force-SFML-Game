#pragma once
#include "Unit.h"
#include "Enums.h"
#include "AudioPlayer.h"
#include "Bullet.h"
#include "DangerZone.h"

#include <future>

class Enemy : public Unit {
protected:
	enum GeneralState{Entry, Logic, Death} genState;

	void LoadSounds(const std::unordered_map<SoundId, sf::SoundBuffer>& _enemySounds);

	bool drawBlur = false;
	sf::Vector2f blurOffset;

	virtual void Entrance(float _deltaTime) = 0;
	sf::Vector2f entrancePos = sf::Vector2f(0,0);
	bool entranceSet = false;

	virtual void UpdatePosition(float _deltaTime) = 0;

	void AnimateLooping(int _animState);
	void AnimateStraight(int _animState);

	void SetHealth(int);

	float difficultyModifier = 1.f;

	virtual void OnDeathLogic();

	//check if money was collected
	int moneyWorth = 0;
	bool wasCashed = false;

	//check if async funcion timer allows for destruction
	bool allowDestory = false;

	std::unique_ptr<std::thread> deathThreadPtr = nullptr;
	bool terminateDeathThread = false;

	std::vector<Bullet*> newBullets;
	virtual void FireBullets() = 0;

	virtual void UniqueOnDamageLogic(bool _wasContact) {};

	//used in unit specific targeting logic
	sf::Vector2f playerPos;
	sf::Vector2f playerPosDiff;

	//used by some enemies for special attacks
	//gunner flamethrower
	DangerZone* dangerZone = nullptr;

public:
	enum EnemyType { Drone, Gunner, Rocketeer, Seeker } type = Drone;

	Enemy(int _id, const std::unordered_map<SoundId, sf::SoundBuffer>& _enemySounds, int _width, int _height, sf::Vector2f _position, const sf::Texture* _texture, sf::Vector2<int> _hitbox, sf::Vector2<int> _hitboxOffset);
	~Enemy();

	virtual void LoadDangerZone(const sf::Texture* _texture) {};

	void TakeDamage(int _value, bool _wasContact);
	bool CheckBlur() const;
	sf::Vector2f GetBlurOffset() const;

	bool AllowDestroy() const;

	EnemyType GetType() const;
	int GetMoney();

	std::vector<Bullet*> GetNewBullets();

	void UpdatePlayerPos(sf::Vector2f _pos);

	DangerZone* GetDangerZonePtr();
	bool CheckDangerZoneCollision(const sf::Vector2f _hitboxPos, const sf::Vector2i _hitbox);
	int GetDangerZoneDamage();
};