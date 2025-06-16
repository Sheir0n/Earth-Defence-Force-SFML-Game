#pragma once

class PlayerPrefs {
public:
	enum WindowTypes { Windowed, Fullscreen, Borderless };


	//music
	static int MENUMUSIC;
	static int INGAMEMUSIC;
	static int EFFECTS;

	//graphics
	static int RESOLUTION;
	static int FRAMERATE;
	static bool VSYNC;
	static WindowTypes WINDOW;

	PlayerPrefs() = delete;

	static void SetDefaults()
	{
		MENUMUSIC = 5;
		INGAMEMUSIC = 5;
		EFFECTS = 5;

		RESOLUTION = 2;
		FRAMERATE = 60;
		VSYNC = true;
		WINDOW = Windowed;
	}
};