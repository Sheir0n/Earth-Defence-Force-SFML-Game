#pragma once
#include "Enemy.h"

class EnemyGunner : public Enemy {
private:
	enum AnimStates { IdleAnim, FiringChargeAnim, FiringAnim, InterruptAnim, FlamethrowerChargeAnim, FlamethrowerAnim, ExplosionAnim, Nothing }animState;
	enum UniqueLogicStates { Wander, FiringCharge, Firing, Interrupt, FlamethrowerCharge, Flamethrower, GoBack } uniqueLogicState;

	void LoadSounds(const std::unordered_map<SoundId, sf::SoundBuffer>& _enemySounds);
	void Entrance(float _deltaTime);

	sf::Vector2f acc;
	//acc clamp
	const float accLimit = 1.8f;
	//acc change
	const float speed = 1.3f;

	void UpdatePosition(float _deltaTime);
	//void SelectNextPos();
	void CalculateAcc(float _deltaTime, float _accModifier);

	//remaining distance
	sf::Vector2f moveDistance;

	//vector value of target position
	sf::Vector2f targetPos;

	void FireBullets();
	float firerateTimer = 0;
	int timeBetweenShots = 175;
	float currFirerateTime = 0;

	void OnDeathLogic() override;
	void UniqueOnDamageLogic(bool _wasContact) override;

	float startFiringTimer = 0;
	int timeToFireMs = 1000;
	//sets timer that calculates logic change to firing
	void SetFireTimer();
	//postpones timer if low on hit
	void PostponeFireOnDamage();
	bool CheckFireTimer() const;
	float timerModifier = 1.f;

	float changeStateTimer = 0;

	//adds 1 when charging interrupted
	int interruptionCount = 0;
	int interruptThreshold = 7;

	//threshold when to stop firng bullets and change logic to wander
	int stopFiringHpThreshold = 0;

	void FindNewWanderPos();
	enum DirectionFloat { NotSet, Up, Down }floatDir = NotSet;
	void FindNewFireFloatPos();

	//used to predict player position
	sf::Vector2f CalculateBulletTargetPos(float _deltaTime);
	float deltaTime = 0.f;

	void FireDeathSalvo();

	bool deathSalvoFired = false;
public:
	EnemyGunner(int _id, const std::unordered_map<SoundId, sf::SoundBuffer>& _enemySounds, sf::Vector2f _position, const sf::Texture* _texture, float _difficulty);

	void LoadDangerZone(const sf::Texture* _texture) override;

	void OnUpdate(float _deltaTime);
	void Animate();
};