#ifndef _WORLD_H_
#define _WORLD_H_

#include <ClanLib/core.h>
#include <ClanLib/sound.h>
#include <list>

#include "gameobject.h"
#include "terrain.h"
#include "enemies.h"
#include "towers.h"
#include "player.h"
#include "menu.h"
#include "towertype.h"

class World
{
//Construction
public:
	World(CL_DisplayWindow &display_window);
	~World(void);

//Attributes
public:
	CL_ResourceManager resources;
	CL_GraphicContext get_gc() { return graphicContext; }

//Operations
public:
	void run();

	void addObject(GameObject *object);
	void enemySurvived(const Enemy& enemy);
	void enemyDied(const Enemy& enemy);

	std::vector<Enemy*> getEnemies() const;
	const CL_Sizex<float>& getFieldSize() const;
	const std::vector<CL_Point>& getWayPoints() const;
	std::vector<TowerType*> getTowerTypes() const;
	int getMoney() const;
	int getLifes() const;
	int getCountdown() const;

//Implementation
private:
	void draw();
	void update();

	int calcTimeElapsed();

	void onWindowClose();
	void onWindowResize(int width, int height);
	void onMouseDown(const CL_InputEvent &key, const CL_InputState &state);
	void onMouseMove(const CL_InputEvent &key, const CL_InputState &state);
	void onKeyDown(const CL_InputEvent &key, const CL_InputState &state);

	void showPreview(int x, int y);
	void setCurrentTowerType(int id);

	CL_DisplayWindow window;
	CL_GraphicContext graphicContext;
	
	std::list<GameObject *> objects;
	std::vector<Enemies*> waves;

	CL_SoundBuffer_Session themeSession;

	Terrain* terrain;
	Towers* towers;
	Player* player;
	Menu* menu;

	int currentTowerType;

	unsigned int currentWave;
	float countdown;

	bool buildTower;
	bool quit;
	bool gameOver;
	bool win;
};

#endif