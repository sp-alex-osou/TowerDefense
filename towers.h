#ifndef _TOWERS_H_
#define _TOWERS_H_

#include "gameobject.h"
#include "towertype.h"
#include "tower.h"
#include "enemy.h"

#include <ClanLib/core.h>
#include <ClanLib/display.h>

class Towers : public GameObject
{
public:
	Towers(World *world);
	~Towers(void);

	virtual void draw();
	virtual bool update(int timeElapsed_ms);

	bool addTower(const CL_Point& position, int type);
	bool addType(int id, const std::string& name, const std::string& texture, const std::string& preview, float speed, float damage, float range, float slow, float splash, const CL_Color& color, int price);
		
	void setFieldSize(CL_Sizex<float> fieldSize);

	bool showPreview(const CL_Point& position, int type);
	void hidePreview();

	bool isFieldEngaged(const CL_Point& position) const;
	std::vector<TowerType*> getTowerTypes() const;
	const TowerType& getTowerType(int id) const;

private:
	typedef std::map<int, TowerType*> mapType;

	mapType towerTypes;
	std::vector<Tower*> towerList;

	CL_Sizex<float> fieldSize;

	struct 
	{
		CL_Texture *texture;
		CL_Point position;
		bool visible;
		float range;
	} preview;
};

#endif