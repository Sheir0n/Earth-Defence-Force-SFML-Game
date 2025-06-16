#pragma once

#include "Player.h"
#include "UIManager.h"
#include "Enemy.h"
#include "EnemyDrone.h"
#include "EnemyGunner.h"
#include "LightRay.h"
#include "Background.h"
#include "Cloud.h"
#include "EnemyRocketeer.h"
#include "EnemySeeker.h"

#include "PlayerPrefs.h"

#include <future>

#include <list>
#include <queue>
#include <mutex>

class GameplayManager {
private:
	sf::RenderWindow* windowPtr;
	Background* background;
	Cloud* clouds[10];

	std::unordered_map<TextureId, sf::Texture> textures;
	void LoadTexture(TextureId _name, std::string _path);

	std::unordered_map<SoundId, sf::SoundBuffer> playerSounds;
	void LoadPlayerSound(SoundId _name, std::string _path);

	bool isPaused = false;
	Player* player;
	//synces animation frame updates
	float unitFrameTimer = 0;

	//one time player inputs
	bool playerDodge = false;
	bool playerShootLeft = false;
	bool playerShootRight = false;

	//mouse cursor
	sf::Vector2f mousePos;
	void UpdateMousePos();

	//bullets lists
	std::list<Bullet*> bullets;

	void ManageBullets(float _deltaTime);
	void CreateNewBullets();

	//locking threads
	std::mutex playerMtx;
	std::mutex enemyMtx;
	std::mutex bulletsMtx;
	std::mutex raysMtx;

	std::mutex bgMtx;
	std::mutex cloudsMtx;

	//top ui stats counters
	float statDistanceFloat = 0;
	int statDistance = 0;
	int statKills = 0;
	int statEarned = 0;
	int statWave = 0;

	//modifier of enemy health and damage dealt
	float difficultyModifier = 1.f;

	//how many enemies in wave
	int waveTotalCapacity = 0;
	//how many enemies at once on screen
	int waveAtOnceCapacity = 0;
	int currUsedCapacity = 0;

	void UpdateStats(float _deltaTime);

	void NewWave();
	std::queue<std::pair<Enemy::EnemyType, float>> enemyQueue;
	void ClearEnemyQueue();

	std::unordered_map<SoundId, sf::SoundBuffer> enemySounds;
	void LoadEnemySound(SoundId _name, std::string _path);

	//enemy capacity
	std::unordered_map<Enemy::EnemyType, int> enemyCap{
		std::pair(Enemy::Drone,1),
		std::pair(Enemy::Gunner,5),
		std::pair(Enemy::Rocketeer,2),
		std::pair(Enemy::Seeker,3),
	};

	//debug 
	//lock enemies of higher diffidulty than:
	//const int debugLimitEnemyTypes = 3;

	void EnemyWaveLogic();
	float spawnDelayTimer = 0;
	bool spawntimerLock = false;

	std::list<Enemy*> enemies;
	int currEnemyId = 1;
	void SpawnEnemy(Enemy::EnemyType);
	void ManageEnemies(float _deltaTime);

	void ManageUi(float _deltaTime, UIManager* _uiManagerPtr);

	void AnimateBullets();
	void AnimateEnemies();

	std::list<LightRay*> deathRays;
	float addRayTimer = 0;
	int rayTimeMs = 350;
	void AddLightRay(sf::Vector2f _pos);

	void DestroyBullets();
	void DestroyEnemies();
	void DestroyRays();
	void DestroyClouds();

	//spawn clouds in this constructor
	void ConstructClouds(bool _inConstructor);

	void SpawnCloud(int _layer, bool _randX);
	float spawnCloudTimer = 0;
	float currSpawntime = 0;
	float spawnThreshold = 2500;

	float pauseTimer = 500;

public:
	enum GameplayState { Logic, PlDead, ShowReturn }logicState = Logic;

	GameplayManager(sf::RenderWindow* _window);
	~GameplayManager();

	void UpdateWindowPtr(sf::RenderWindow* _window);

	void UpdateAnimator(float _deltaTime);

	void NewGame(UIManager* _uiManagerPtr, std::pair<int, int> _weapons);

	enum PlayerInput { Dodge, ShootLeft, ShootRight };
	void PassPlayerInputEvent(PlayerInput _input);

	void DrawBackgroundObjects(sf::RenderWindow* _windowPtr);
	void DrawGameObjects(sf::RenderWindow* _windowPtr);
	void HandleGameLogic(float _deltaTime, UIManager* _uiManagerPtr);
	void HandleBackgroundLogic(float _deltaTime, UIManager* _uiManagerPtr);

	std::tuple<int&, int&, int&, int&> const GetGameOverStats();

	GameplayState GetState() const;
	bool IsGamePaused() const;
	void Pause();
};