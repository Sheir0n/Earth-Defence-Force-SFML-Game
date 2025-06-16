#pragma once
#include "GameObject.h"
#include "Enums.h"

#include <iostream>
#include <string>
#include <unordered_map>
#include <random>
#include <chrono>
#include <SFML/Graphics.hpp>

class Bullet : public GameObject {
protected:
	sf::Sprite sprite;
	int frameCount = 0;

	int damage = 1;
	float speed = 1;
	float angleDeg = 0;
	float angleRad = 0;
	float inaccuracyInDeg = 0;
	int penetration = 0;

	bool activeHitbox = false;
	float hitboxSize = 1;
	bool despawn = false;

	int animFrameCount = 1;
	int animFrame = 1;

	float ConvertToDeg(float _rad);
	float ConvertToRad(float _deg);

	std::unordered_map<int, int> idsHit;

	float spawnTimer = 0;

	void LoadSpriteData();

	float CalculateRotation(sf::Vector2f _pos);
	float CalculateRotation(float _angDeg);

	sf::Vector2f targetPos;

	int logicCheckSteps = 3;
public:
	enum Type { BlasterB, SpitterB, RailB, RocketB, DroneB, GunnerB, RocketeerB, RocketeerR, SeekerB } bulletType;
	enum Owner { Player, Enemy } owner;

	Bullet(int _width, int _height, sf::Vector2f _position, Type _type);

	void OnUpdate(float _deltaTime) {};

	//load data 
	//angle depending on target pos
	void LoadData(sf::Vector2f _pos, sf::Vector2f _targetPos, bool _activeHitbox);

	//static angle
	void LoadData(sf::Vector2f _pos, float _roationDeg, bool _activeHitbox);

	void LoadTextures(std::unordered_map<TextureId, sf::Texture>& _textures);

	const sf::Sprite& GetDrawData();

	virtual std::vector<sf::Vector2f> UpdatePosition(float _deltaTime);

	float GetRadRotation() const;

	bool CheckDespawn() const;

	void Animate();

	//used in calculating hits
	sf::Vector2i GetHitbox() const;
	Owner GetOwner() const;
	//used for checking if unit id was hit recently
	bool CheckIfIdHit(int _id, float _deltaTime);
	void AddId(int _id);

	//lower bullet penetration, if < 0 set to despawn
	void LowerPen();
	int GetDamage() const;

	void ApplyModifier(float _modifier);

	void UpdateTargetPos(sf::Vector2f _target);
};