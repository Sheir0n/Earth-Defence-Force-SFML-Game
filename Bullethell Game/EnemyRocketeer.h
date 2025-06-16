#pragma once
#include "Enemy.h"
#include "SinusoidBullet.h"
#include "RocketBullet.h"

class EnemyRocketeer : public Enemy {
private:
	enum AnimStates { IdleAnim, RocketfireAnim, GunfireAnim, ExplosionAnim, Nothing }animState;
	enum UniqueLogicStates { Idle, Rocketfire, Gunfire, Dodge } uniqueLogicState;

	void LoadSounds(const std::unordered_map<SoundId, sf::SoundBuffer>& _enemySounds);
	void Entrance(float _deltaTime);

	void CalculateAcc(float _deltaTime, float _accModifier);
	void UpdatePosition(float _deltaTime);

	sf::Vector2f moveDistance;
	sf::Vector2f targetPos;
	sf::Vector2f acc;
	const float accLimit = 1.6f;
	const float speed = 2.5f;

	void FireBullets();

	float deltaTime = 0.f;
	void OnDeathLogic() override;

	bool enableDodge = false;

	void FindNewIdlePos();
	float moveCooldownTimer = 0;
	int moveCooldownInMs = 2000;
	bool startedMoveTimer = false;

	void FindDodgePos();
	void CalculateDodgeAcc();
	float dodgeCooldownTimer = 0;
	bool firstTimeDodge = true;
	int dodgeCooldown = 4000;

	int fireMoveCount = 0;
	int fireMoveThreshold = 4;

	UniqueLogicStates SetFireState();

	float changeStateTimer = 0;
	float fireTime = 0;
	int bulletCount = 0;
	bool parityCheck = false;

	void RandomiseParity();
	void UniqueOnDamageLogic(bool _wasContact) override;

public:
	EnemyRocketeer(int _id, const std::unordered_map<SoundId, sf::SoundBuffer>& _enemySounds, sf::Vector2f _position, const sf::Texture* _texture, float _difficulty);
	void OnUpdate(float _deltaTime);
	void Animate();

	//checks if any bullets near if true enable dodge motion
	bool EnableDodgeInDanger(sf::Vector2f _pos);
};


