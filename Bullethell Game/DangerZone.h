#pragma once
#include "GameObject.h"
#include <unordered_map>

class DangerZone : public GameObject {
private:
	sf::Vector2f dangerZone;
	sf::Sprite sprite;
	bool active = false;

	int damage = 0;
	float difficultyModifier = 1.f;

	std::unordered_map<int, std::pair<int, int>> animationData;
	sf::Vector2f hitboxPos;
	sf::Vector2f hitbox;
	sf::Vector2f fixedHitbox;

	int animState = 0;
	int animationFrame = 1;
	std::vector<int> stopAnimFrames;

public:
	enum DangerType { Flame,Laser }dangerType;

	DangerZone(DangerType _type, int _width, int _height, sf::Vector2f _position, const sf::Texture* _texture, float _difficultyModifier);

	void OnUpdate(float _deltaTime) {};

	const sf::Sprite& GetDrawData();

	void Activate(bool _value);
	void UpdatePosition(sf::Vector2f _pos);

	void Animate();

	bool CheckCollision(const sf::Vector2f _hitboxPos, const sf::Vector2i _hitbox);

	int GetDamage() const;

	void SetFrameState(int _animState);
};