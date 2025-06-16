#pragma once
#include "GameObject.h"
#include <string>

class UIObject : public GameObject {
private:
	sf::Sprite sprite;
	std::string name;
	bool isClickable = false;
	bool isHovered = false;
	int frame = 1;

	//button default frame
	//used for multitexture buttons
	int baseFrame = 1;

	sf::Vector2<int> constraints;
public:
	UIObject() = delete;
	UIObject(int _width, int _height, sf::Vector2i _position);
	UIObject(int _width, int _height, sf::Vector2i _position, bool _isActive);
	UIObject(int _width, int _height, sf::Vector2i _position, int _frameCount);
	~UIObject();
	void OnUpdate(float _deltaTime) {};

	const sf::Sprite& GetDrawData();

	void SetClickable(bool _isClickable);
	void SetTexture(const sf::Texture* _texture);
	void SetTexture(const sf::Texture* _texture, sf::Vector2<int> _constraints);

	void UpdateHoverSprite(sf::Vector2f _mousePos);

	bool CheckIfHovered() const;

	void SetFrame(int _frame);

	void SetColor(sf::Color _color);
	void Flip(bool _flip);
	void SetBaseFrame(int _frame);
	int GetBaseFrame() const;
};

