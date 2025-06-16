#pragma once
#include "GameObject.h"

class Background : public GameObject {
private:
	sf::Sprite sprite;
	int animationFrame = 1;

public:
	Background(const sf::Texture* _texture);
	void OnUpdate(float _deltaTime) {};
	void Animate();
	const sf::Sprite& GetDrawData();

};