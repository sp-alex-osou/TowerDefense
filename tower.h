#ifndef _TOWER_H_
#define _TOWER_H_

#include "gameobject.h"
#include "towertype.h"
#include "enemy.h"
#include "missile.h"

#include <ClanLib/display.h>
#include <ClanLib/core.h>

class Tower : public GameObject
{
public:
	Tower(World* world, const CL_Point& position, const TowerType& type);
	~Tower();

	virtual bool update(int timeElapsed_ms);

	const CL_Point& getPosition() const;
	const TowerType& getType() const;
	const std::vector<Missile*>& getMissiles() const;

private:
	Enemy* getTarget() const;

	std::vector<Missile*> missiles;
	CL_Point position;
	const TowerType& type;
	Enemy* target;
	float lock;
};

#endif