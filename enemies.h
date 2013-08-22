#ifndef _ENEMIES_H_
#define _ENEMIES_H_

#include <ClanLib/core.h>

#include "gameobject.h"
#include "enemy.h"

class CL_Sprite;
class CL_SoundBuffer;

class Enemies : public GameObject
{
// Construction
public:
	~Enemies();
	static Enemies* getInstance(World *world, const CL_StringRef& type);
	
// Operations
public:	
	virtual void draw();
	virtual bool update(int timeElapsed_ms);
	
	void setFieldSize(CL_Sizex<float> fieldSize);

	const std::vector<Enemy*>& getEnemies() const;
	bool isBoss() const;

// Implementation:
private:
	Enemies(World *world, const CL_StringRef& spriteName, float speed, float hitpoints, int value, bool boss);

	CL_Sprite *sprite;	

	CL_Sizex<float> fieldSize;
	float hitpoints;
	int value;
	bool boss;

	unsigned int count;
	int interval;

	std::vector<Enemy*> enemies;
};

#endif

