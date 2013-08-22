#ifndef _ENEMYTYPE_H_
#define _ENEMYTYPE_H_

#include <ClanLib/core.h>
#include <ClanLib/display.h>

struct EnemyType
{
	std::string name;

	float speed;
	float hitpoints;

	CL_Texture texture;

	TowerType(const std::string& name, const CL_Texture& texture, float speed, float hitpoints)
		: name(name), texture(texture), speed(speed), hitpoints(hitpoints)
	{
	}
};

#endif