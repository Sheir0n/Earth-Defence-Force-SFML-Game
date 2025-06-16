#pragma once

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <regex>

#include "UIManager.h"

class PlayerProfile {
private:
	std::string name;
	int balance;
	int highestWave;
	int noMissions;
	int killCount;

	std::filesystem::path saveFilePath;
	//add container for unlocked parts
	int weaponUnlocks[8]{ 0,0,2,2,2,2,2,2 };
	enum WeaponUnlock {
		Equipped, Inv, Locked
	};

	void ReadSaveData();
	void WriteSaveData();
	void UpdateSaveFileToNewVersion(int _saveVersion);

	const int weaponCost[4]{ 0,5000,7000,10000 };

public:
	PlayerProfile() = delete;
	PlayerProfile(std::string _name);

	std::string GetName();

	void WriteStatsToUI(UIManager* _managerPtr);

	void UpdateData(std::tuple<int, int, int, int>);
	void EquipWeapon(int _weaponId, UIManager* _managerPtr);

	std::pair<int, int> GetEquippedWeapons()const;
};