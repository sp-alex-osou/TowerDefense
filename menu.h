#ifndef _MENU_H_
#define _MENU_H_

#include <ClanLib/core.h>
#include <ClanLib/display.h>

#include "gameobject.h"
#include "towertype.h"

class Menu : public GameObject
{
// Constructor
public:
	Menu(World *world, CL_Rect viewPort);
	~Menu(void);

// Operations
public:
	virtual void draw();
	virtual bool update(int timeElapsed_ms);

	int getMenuHeight() const;

	void setViewPort(int width, int height);

	int getMenuField(int x, int y) const;

private:
	const int menuHeight;
	const int menuTowerLength;
	const int menuCoinsLength;
	const int menuTimeLength;
	const int menuLifeLength;

	int menuWidth;

	CL_Texture coinsTexture;
	CL_Texture clockTexture;
	CL_Texture heartTexture;

	std::vector<TowerType*> towerTypes;

	CL_Rect viewPort;

	void drawMenuItem(std::string output, int itemLength, CL_Texture texture, CL_Font font,
		int textOffsetY, int xPosition);
};

#endif