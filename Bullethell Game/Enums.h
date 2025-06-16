#pragma once

enum GameState { MainMenu, InfoMenu, SettingsMenu, CreditsMenu, LoginScreen, ProfileScreen, GameScreen, GameOverScreen };

enum TextureId {
	//UI

	//main menu
	MainBanner, MainPlayButton, MainInfoButton, MainSettingsButton, MainCreditsButton, MainExitButton,
	//info menu
	InfoBanner, InfoReturn,
	//credits menu
	CreditsBanner,
	//login window
	LoginWindow, LoginReturn, SignIn,
	//profile screen
	ProfileLogo, ProfileStats, ProfileParts, ProfilePlayButton, ProfileLogoutButton, ProfileExitButton, ProfileWeaponButtons, ProfileExtras, WeaponBar,

	//Settings Screen
	SettingsWindow, ResetButton,
	SliderDec, SliderInc, Slider,
	ButResolution1, ButResolution2, ButResolution3, ButResolution4,
	ButFps30, ButFps60, ButFps75, ButFps144, ButFpsUnlimited,
	ButVsyncOff, ButVsyncOn,
	ButWindowed, ButFullscreen, ButBorderless,

	//Game ober screen
	GameOver,

	//in game
	GameUIBg, GameUIShip, GameUIWarning, GameUITopStats,
	//weapons
	Guns, WeaponBlaster,
	//Death anim ray
	Ray,

	//Mouse
	Cursor,


	//GAMEPLAY
	PlayerTxt,

	//Bullets
	BlasterBullet,
	SpitterBullet,
	PiercerBullet,
	LauncherBullet,

	DroneBullet,
	GunnerBullet,
	RocketeerBullet,
	RocketeerRocket,
	SeekerBullet,

	//Enemies
	DroneTxt, GunnerTxt, RocketeerTxt, SeekerTxt,

	//Danger Zones
	FlamethrowerZone, LaserZone,

	//Backgrounds
	BackgroundTxt, CloudSmall, CloudMedium, CloudLarge,

};

enum SoundId {
	//Songs
	MenuSong, InGameSongLoop,
	//PLAYER
	PlEntrance,
	PlDodge1, PlDodge2, PlDodge3,
	PlHitShield, PlHitHealth, PlShieldDown, PlShieldRegain,
	PlDeath,
	//WEAPON
	WpReloadComplete, WpBlasterFire, WpSpitterFire, WpPiercerFire, WpSeekerFire,

	//ENEMY unique
	DroneFire, DroneRam,
	GunnerDeploy, GunnerFire, GunnerInterrupt, GunnerFlamethrower,
	RocketeerDodge, RocketeerGunfire, RocketeerRocketfire,
	SeekerDeath, SeekerFireball, SeekerLaser,

	//generic tag to be used in different enemies
	//not assigned sound at the start
	EnemyHit,

	//instead of above load theese
	DroneHit,

	//generic tags with assigned sounds
	EnemyDeath,
};