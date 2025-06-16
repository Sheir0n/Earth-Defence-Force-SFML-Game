#pragma once
#include "Enemy.h"
#include "FIreballBullet.h"

class EnemySeeker : public Enemy {
private:
	enum AnimStates { IdleAnim, FireballAnim, LaserPrepAnim, LaserFireAnim, LaserEndAnim, ExplosionAnim, Nothing }animState;
	enum UniqueLogicStates { Idle, Fireball, LaserPrep, Laser, EndLaser } uniqueLogicState;

	void LoadSounds(const std::unordered_map<SoundId, sf::SoundBuffer>& _enemySounds);
	void Entrance(float _deltaTime);

	void CalculateAcc(float _deltaTime, float _modifier, bool _seek);
	void UpdatePosition(float _deltaTime);

	sf::Vector2f moveDistance;
	sf::Vector2f targetPos;
	sf::Vector2f acc;

	float accLimit = 0.5f;
	float speed = 1.5f;

	void FireBullets();
	void OnDeathLogic() override;

	void RandomiseAcc();
	bool randomised = false;

	bool startedTargeting = false;

	float targetingTimer = 0.f;
	int targetingFireThreshold = 425;
	int targetingLaserThreshold = 1000;

	float stateChangeTimer = 0.f;

	bool fired = false;

	void UniqueOnDamageLogic(bool _wasContact) override;
public:
	EnemySeeker(int _id, const std::unordered_map<SoundId, sf::SoundBuffer>& _enemySounds, sf::Vector2f _position, const sf::Texture* _texture, float _difficulty);

	void LoadDangerZone(const sf::Texture* _texture) override;

	void OnUpdate(float _deltaTime);
	void Animate();
};
