#pragma once
#include "GameObject.h"

class Crosshair : public GameObject {
private:
	sf::Vector2i pointOffset;

	sf::Sprite sprite;
	int frame = 1;
	bool inGameOffset = false;
public:
	Crosshair(const sf::Texture* _texture);
	
	void OnUpdate(float _deltaTime) {};

	void UpdatePos(sf::Vector2f _pos);

	const sf::Sprite& GetDrawData();

	void SetFrame(int _frame);

	void SetInGameOffset(bool _isInGame);
};