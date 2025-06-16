#include "Weapon.h"

Weapon::Weapon(Weapon::WeaponType _type, const std::unordered_map<SoundId, sf::SoundBuffer>& _playerSounds)
{
	this->type = _type;

	//initializing weapon data
	switch (_type) {
	case Blaster:
		this->maxAmmo = 12;
		this->reloadTimeInMs = 2000;
		this->timeBetweenShotsInMs = 225;
		this->isAutomatic = false;
		this->weaponFireSound = WpBlasterFire;
		break;
	case Spitter:
		this->maxAmmo = 60;
		this->reloadTimeInMs = 5750;
		this->timeBetweenShotsInMs = 120;
		this->startFireSlowdown = 200;
		this->isAutomatic = true;
		this->weaponFireSound = WpSpitterFire;
		break;
	case RailPiercer:
		this->maxAmmo = 3;
		this->reloadTimeInMs = 3250;
		this->timeBetweenShotsInMs = 1000;
		this->isAutomatic = false;
		this->weaponFireSound = WpPiercerFire;
		break;
	case FoeSeeker:
		this->maxAmmo = 8;
		this->reloadTimeInMs = 2800;
		this->timeBetweenShotsInMs = 200;
		this->isAutomatic = true;
		this->weaponFireSound = WpSeekerFire;
		break;
	default:
		this->maxAmmo = 12;
		this->reloadTimeInMs = 3000;
		this->timeBetweenShotsInMs = 240;
		this->isAutomatic = false;
		this->weaponFireSound = WpBlasterFire;
		break;
	}

	this->ammo = maxAmmo;

	this->reloadTimer = 0;
	this->fireTimer = 0;

	this->audioPlayer = new AudioPlayer;

	//loading sounds
	this->audioPlayer->LoadSound(WpReloadComplete, &_playerSounds.at(WpReloadComplete), 25, 1.f);

	switch (_type) {
	case Blaster:
		this->audioPlayer->LoadSound(WpBlasterFire, &_playerSounds.at(WpBlasterFire), 37, 1.f);
		break;
	case Spitter:
		this->audioPlayer->LoadSound(WpSpitterFire, &_playerSounds.at(WpSpitterFire), 55, 1.f);
		break;
	case RailPiercer:
		this->audioPlayer->LoadSound(WpPiercerFire, &_playerSounds.at(WpPiercerFire), 65, 1.f);
		break;
	case FoeSeeker:
		this->audioPlayer->LoadSound(WpSeekerFire, &_playerSounds.at(WpSeekerFire), 55, 1.25f);
		break;
	}

}

Weapon::~Weapon()
{
	delete this->audioPlayer;
}


bool Weapon::IsReady()
{
	this->firingPressed = true;

	if (!isReloading) {
		if (this->timeBetweenShotsInMs + this->startFireSlowdown < fireTimer)
			return true;
		else
			return false;
	}
	else
		return false;
}

Bullet* Weapon::Fire()
{
	if (IsReady())
	{
		this->audioPlayer->PlaySoundId(weaponFireSound, PlayerPrefs::EFFECTS * 0.1f);
		this->fireTimer = 0;

		this->ammo--;

		if (this->ammo <= 0) {
			this->isReloading = true;
			this->reloadTimer = 0;
		}
	}

	return nullptr;
}

bool Weapon::IsAutomatic() const
{
	return this->isAutomatic;
}

std::pair<int, int> Weapon::GetAmmoData()
{
	return std::make_pair(this->maxAmmo, this->ammo);
}

void Weapon::UpdateReloadStatus()
{
	if (!isReloading)
		return;

	if (this->reloadTimeInMs < this->reloadTimer) {
		this->ammo = this->maxAmmo;
		this->isReloading = false;
		this->audioPlayer->PlaySoundId(WpReloadComplete, PlayerPrefs::EFFECTS * 0.1f);
	}
}

void Weapon::ForceReload()
{
	if (ammo < maxAmmo) {
		this->ammo = 0;
		this->isReloading = true;
		this->reloadTimer = 0;
	}
}

Weapon::WeaponType Weapon::GetWeaponType() const
{
	return this->type;
}

void Weapon::OnUpdate(float _deltaTime)
{
	this->reloadTimer += _deltaTime * 1000;
	this->fireTimer += _deltaTime * 1000;

	if (this->type == Spitter) {
		if (this->firingPressed && !isReloading) {
			this->startFireSlowdown -= startFireSlowdownReduction * _deltaTime;
			this->startFireSlowdown = std::clamp(this->startFireSlowdown, 0.f, 1000.f);
		}
		else
			this->startFireSlowdown = 200;

		this->firingPressed = false;
	}


	this->UpdateReloadStatus();
}
