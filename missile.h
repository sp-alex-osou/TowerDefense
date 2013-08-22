#ifndef _MISSILE_H_
#define _MISSILE_H_

#include "gameobject.h"
#include "enemy.h"

class Missile : public GameObject
{
public:
	Missile(World *world, const CL_Pointx<float>& position, Enemy& target, float damage, float slow, float splash, const CL_Color& color);

	virtual bool update(int timeElapsed_ms);

	const CL_Pointx<float>& getPosition() const;

private:
	CL_Pointx<float> position;
	Enemy& target;
	float damage;
	float slow;
	float duration;
	float splash;
	CL_Color color;

	void hitTargets();
};

#endif
