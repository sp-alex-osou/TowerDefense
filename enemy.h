#ifndef _ENEMY_H_
#define _ENEMY_H_

#include <ClanLib/core.h>
#include <ClanLib/display.h>

#include "gameobject.h"

class Enemy : public GameObject
{
// Construction
public:
	Enemy(World *world, const std::vector<CL_Point>& wayPoints, float speed, float hitpoints, int value, bool boss, bool alive);

// Operations
public:
	virtual bool update(int timeElapsed_ms);

	void setSpeed(float speed);
	void setAlive(bool alive);
	void hit(float damage, float slow, const CL_Color& color);

	const CL_Angle& getAngle() const;
	const CL_Pointx<float>& getPosition() const;
	bool isAlive() const;
	bool isSlowed() const;
	bool isBoss() const;
	float getHitpoints() const;
	int getValue() const;
	const CL_Color& getColor() const;

// Implementation:
private:	
	CL_Angle angle;
	float defaultSpeed;
	float speed;
	float hitpoints;
	int value;
	bool alive;
	bool boss;
	CL_Color color;

	float hitInterval;

	float slow;
	float duration;

	CL_Pointx<float> position;
	CL_Pointx<float> target;

	std::vector<CL_Point> wayPoints;

	void setPosition(CL_Point wayPoint);
	void setTarget(CL_Point wayPoint);
	void move(float distance);
};

#endif

