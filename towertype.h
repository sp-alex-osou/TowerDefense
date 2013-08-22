#ifndef _TOWERTYPE_H_
#define _TOWERTYPE_H_

#include <ClanLib/core.h>
#include <ClanLib/display.h>

struct TowerType
{
	std::string name;

	float speed;
	float damage;
	float range;
	float slow;
	float splash;
	int price;
	int id;
	
	CL_Color color;

	CL_Texture texture;
	CL_Texture preview;

	TowerType(
		int id,
		const std::string& name, 
		const CL_Texture& texture, 
		const CL_Texture& preview, 
		float speed, 
		float damage, 
		float range, 
		float slow, 
		float splash,
		const CL_Color& color,
		int price)
	: 
		id(id),
		name(name), 
		texture(texture), 
		preview(preview), 
		speed(speed), 
		damage(damage), 
		range(range), 
		slow(slow), 
		splash(splash), 
		color(color),
		price(price)
	{
	}
};

#endif
