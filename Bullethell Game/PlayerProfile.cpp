#include "PlayerProfile.h"


//private
void PlayerProfile::ReadSaveData()
{

	std::string _varName;
	std::string _data;

	std::vector<int> _dataVector;


	std::fstream _file;
	_file.open(this->saveFilePath, std::fstream::in);

	//parsing csv file
	try {
		if (_file.is_open()) {

			std::string _line;
			while (std::getline(_file, _line)) {
				std::replace(_line.begin(), _line.end(), ';', ' ');
				std::stringstream _dataStream(_line);
				if (!(_dataStream >> _varName >> _data)) {
					throw std::string("file corrupted");
				}
				else {
					if (std::regex_match(_data, std::regex("^-?[0-9]+$")) || std::stoi(_data) < 0)
						_dataVector.push_back(std::stoi(_data));
					else
						throw std::string("file corrupted");
				}
			}
		}
		else
			throw std::string("file missing");


		if (_dataVector.size() == 4) {
			this->UpdateSaveFileToNewVersion(1);
			_file.close();

			//recurency after fixing file
			this->ReadSaveData();
			return;
		}

		if (_dataVector.size() == 12) {
			this->balance = _dataVector[0];
			this->highestWave = _dataVector[1];
			this->noMissions = _dataVector[2];
			this->killCount = _dataVector[3];

			for (int i = 0; i < 8; i++) {
				this->weaponUnlocks[i] = _dataVector[4 + i];
			}
		}
		else
			throw std::string("file corrupted");

	}
	catch (std::string _errName) {
		std::cerr << "Could not open file: " << _errName << "\nClosing aplication!";
		_file.close();
		exit(-1);
	}
	_file.close();
}


void PlayerProfile::WriteSaveData()
{
	std::fstream _file;
	_file.open(this->saveFilePath, std::fstream::out);
	_file << "_balance_;" + std::to_string(balance) + "\n";
	_file << "_highWave_;" + std::to_string(highestWave) + "\n";
	_file << "_noMissions_;" + std::to_string(noMissions) + "\n";
	_file << "_kills_;" + std::to_string(killCount) + "\n";

	_file << "_w_blaster_l_;" + std::to_string(weaponUnlocks[0]) + "\n";
	_file << "_w_blaster_r_;" + std::to_string(weaponUnlocks[1]) + "\n";
	_file << "_w_spitter_l_;" + std::to_string(weaponUnlocks[2]) + "\n";
	_file << "_w_spitter_r_;" + std::to_string(weaponUnlocks[3]) + "\n";
	_file << "_w_rail_l_;" + std::to_string(weaponUnlocks[4]) + "\n";
	_file << "_w_rail_r_;" + std::to_string(weaponUnlocks[5]) + "\n";
	_file << "_w_seeker_l_;" + std::to_string(weaponUnlocks[6]) + "\n";
	_file << "_w_seeker_r_;" + std::to_string(weaponUnlocks[7]) + "\n";
	_file.close();
}

void PlayerProfile::UpdateSaveFileToNewVersion(int _saveVersion)
{
	std::fstream _file;
	std::fstream _fileOld;
	_fileOld.open(this->saveFilePath, std::fstream::in);
	if (_saveVersion == 1) {
		//creating vector of old lines
		std::vector<std::string> _lines;
		std::string _line;
		while (std::getline(_fileOld, _line)) {
			_lines.push_back(_line);
		}

		//rewriting old lines
		_file.open(this->saveFilePath, std::fstream::out);
		for (std::string _newline : _lines) {
			_file << _newline + "\n";
		}

		_file << "_w_blaster_l_;" + std::to_string(weaponUnlocks[0]) + "\n";
		_file << "_w_blaster_r_;" + std::to_string(weaponUnlocks[1]) + "\n";
		_file << "_w_spitter_l_;" + std::to_string(weaponUnlocks[2]) + "\n";
		_file << "_w_spitter_r_;" + std::to_string(weaponUnlocks[3]) + "\n";
		_file << "_w_rail_l_;" + std::to_string(weaponUnlocks[4]) + "\n";
		_file << "_w_rail_r_;" + std::to_string(weaponUnlocks[5]) + "\n";
		_file << "_w_seeker_l_;" + std::to_string(weaponUnlocks[6]) + "\n";
		_file << "_w_seeker_r_;" + std::to_string(weaponUnlocks[7]) + "\n";
		_file.close();
	}
	_fileOld.close();
}



//public
PlayerProfile::PlayerProfile(std::string _name)
{
	this->name = _name;

	std::string _appdata = getenv("APPDATA");

	this->saveFilePath = std::filesystem::path{ _appdata + "\\EarthDefenceForce" };
	this->saveFilePath /= _name + ".csv";

	if (std::filesystem::exists(saveFilePath)) {
		std::cout << "opened save file\n";
		ReadSaveData();
	}
	else {
		std::cout << "created save file\n";
		this->balance = 0;
		this->highestWave = 0;
		this->noMissions = 0;
		this->killCount = 0;
		for (int i = 2; i < 8; i++)
			this->weaponUnlocks[i] = WeaponUnlock::Locked;

		std::filesystem::create_directories(this->saveFilePath.parent_path());

		this->WriteSaveData();
	}
}

std::string PlayerProfile::GetName()
{
	return this->name;
}

void PlayerProfile::WriteStatsToUI(UIManager* _managerPtr)
{
	//Basic stats
	_managerPtr->EditText(0, std::to_string(this->killCount));
	_managerPtr->EditText(1, std::to_string(this->noMissions));
	_managerPtr->EditText(2, std::to_string(this->highestWave));

	//Rating
	if (noMissions == 0) {
		_managerPtr->EditText(3, "--");
		_managerPtr->EditText(3, sf::Color(139, 155, 180));
	}
	else {
		double _ratingCalc = (double)this->killCount / (double)this->noMissions;

		if (_ratingCalc <= 10) {
			_managerPtr->EditText(3, "D");
			_managerPtr->EditText(3, sf::Color(255, 0, 68));
		}
		else if (_ratingCalc <= 25) {
			_managerPtr->EditText(3, "C");
			_managerPtr->EditText(3, sf::Color(247, 118, 34));
		}
		else if (_ratingCalc <= 50) {
			_managerPtr->EditText(3, "B");
			_managerPtr->EditText(3, sf::Color(254, 231, 97));
		}
		else if (_ratingCalc <= 75) {
			_managerPtr->EditText(3, "A");
			_managerPtr->EditText(3, sf::Color(99, 199, 77));
		}
		else {
			_managerPtr->EditText(3, "S");
			_managerPtr->EditText(3, sf::Color(44, 232, 245));
		}
	}


	//Balance
	_managerPtr->EditText(4, std::to_string(this->balance));

	//buttons
	_managerPtr->UpdateProfileGunButtons(this->weaponUnlocks, this->weaponCost, this->balance);

	//username
	_managerPtr->EditText(6, this->name);
}

void PlayerProfile::UpdateData(std::tuple<int, int, int, int> _data)
{
	this->killCount += std::get<1>(_data);
	this->balance += std::get<2>(_data);
	this->noMissions++;

	if (this->highestWave < std::get<3>(_data))
		this->highestWave = std::get<3>(_data);


	this->WriteSaveData();
}

void PlayerProfile::EquipWeapon(int _weaponId, UIManager* _managerPtr)
{
	if (this->weaponUnlocks[_weaponId] == 0)
		return;
	else if (this->weaponUnlocks[_weaponId] == 1) {
		//iterate over weapons on side unequipping all
		for (int i = _weaponId % 2; i < 8; i += 2) {
			if (this->weaponUnlocks[i] == 0)
				this->weaponUnlocks[i] = 1;
		}
		this->weaponUnlocks[_weaponId] = 0;
	}
	else if (this->weaponUnlocks[_weaponId] == 2) {
		int _cost = this->weaponCost[(int)std::ceil(_weaponId / 2)];
		if (_cost > this->balance)
			return;
		else
			this->balance -= _cost;

		for (int i = _weaponId % 2; i < 8; i += 2) {
			if (this->weaponUnlocks[i] == 0)
				this->weaponUnlocks[i] = 1;
		}
		this->weaponUnlocks[_weaponId] = 0;
	}

	this->WriteSaveData();
	_managerPtr->UpdateProfileGunButtons(this->weaponUnlocks, this->weaponCost, this->balance);
	_managerPtr->EditText(4, std::to_string(this->balance));
}

std::pair<int, int> PlayerProfile::GetEquippedWeapons() const
{
	int _left = 0, _right = 0;

	for (int i = 0; i < 8; i += 2)
		if (this->weaponUnlocks[i] == 0)
			_left = i / 2;

	for (int i = 1; i < 8; i += 2)
		if (this->weaponUnlocks[i] == 0)
			_right = (i-1) / 2;

	return std::make_pair(_left, _right);
}
