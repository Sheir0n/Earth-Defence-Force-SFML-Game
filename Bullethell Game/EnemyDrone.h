#pragma once
#include "Enemy.h"

class EnemyDrone : public Enemy {
private:
	enum AnimStates { Idle, Powerup, Charge, Explosion, ExplosionRam, Nothing }animState;

	enum UniqueLogicStates {Regular, Targeting, Ramming} uniqueLogicState;
	void LoadSounds(const std::unordered_map<SoundId, sf::SoundBuffer>& _enemySounds);

	void Entrance(float _deltaTime);
	//acceleration

	sf::Vector2f acc;
	//acceleration clamp
	const float accLimit = 2.3f;
	//acceleration change speed
	const float speed = 3.f;

	void UpdatePosition(float _deltaTime);

	void SelectNextPos();
	void CalculateAcc(float _deltaTime);

	float moveCooldownTimer = 0;
	int moveCooldownInMs = 2000;
	bool startCooldownTimer = true;

	//remaining distance
	float moveDistance = 0;

	//y value of target position
	int targetPos;

	float timerModifier = 1.f;

	void OnDeathLogic() override;

	void FireBullets();

	void UniqueOnDamageLogic(bool _wasContact) override;

	//make a roll if ramming should be activated
	bool rolled = false;
	bool RollToRam();

	//target player to ram
	void TargetingRotation(float _deltaTime);
	float currRotation = 0.f;
	float rotationModifier = 0.f;
	sf::Vector2f ramTargetPos;
	float targetingTimer = 0;

	//ramming acceletatoin function
	void RammingAcc(float _deltaTime);

public:
	EnemyDrone(int _id, const std::unordered_map<SoundId, sf::SoundBuffer>& _enemySounds, sf::Vector2f _position, const sf::Texture* _texture, float _difficulty);

	void OnUpdate(float _deltaTime);
	void Animate();
};