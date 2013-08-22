#include <ClanLib/display.h>
#include <ClanLib/core.h>
#include <ClanLib/sound.h>

#include "tower.h"
#include "world.h"
#include "towertype.h"
#include "missile.h"

Tower::Tower(World* world, const CL_Point& position, const TowerType& type)
: GameObject(world), position(position), type(type), target(0), lock(0)
{
}

Tower::~Tower()
{
	for (std::vector<Missile*>::iterator it = missiles.begin(); it != missiles.end(); ++it)
		delete (*it);
}

//Update der Türme
bool Tower::update(int timeElapsed_ms)
{
	//Alle Geschosse durchlaufen
	std::vector<Missile*>::iterator it = missiles.begin();
	while (it != missiles.end())
	{
		//Ist das Geschoß bereits am Ziel angekommen, kann dieses aus der Liste gelöscht werden
		if (!(*it)->update(timeElapsed_ms))
		{
			delete (*it);
			it = missiles.erase(it);
		}
		else
			++it;
	}

	lock -= timeElapsed_ms;
	target = getTarget();

	//Ist ein Ziel festgelegt und der Turm wieder schussbereit
	while (lock <= 0.0f && target != 0)
	{
		//Anfangsposition eines Geschosses setzen
		CL_Pointx<float> missilePosition = position;
		missilePosition.x += 0.5f;
		missilePosition.y += 0.5f;
		
		//Geschoss erzeugen und abschießen
		Missile *missile = new Missile(world, missilePosition, *target, type.damage, type.slow, type.splash, type.color);
		missile->update((int)-lock);
		missiles.push_back(missile);

		//Wartezeit für nächsten Schuss setzen
		lock += 1000.0f / type.speed;

		//Neues Target holen
		target = getTarget();
	}

	if (target == 0 && lock < 0.0f)
		lock = 0.0f;

	return true;
}

//Liefert ein Ziel für einen Turm
Enemy* Tower::getTarget() const
{
	const std::vector<Enemy*>& enemies = world->getEnemies();
	std::vector<Enemy*>::const_iterator it;

	std::vector<Enemy*> targets;

	Enemy* target = 0;

	//Gegner durchlaufen
	for (it = enemies.begin(); it != enemies.end(); ++it)
	{
		//Wenn Gegner bereits tot ist, dann nächsten Schleifendurchgang starten
		if (!(*it)->isAlive())
			continue;

		//Gegnerposition ermitteln
		CL_Pointx<float> enemyPosition = (*it)->getPosition();
		//Turmposition setzen
		CL_Pointx<float> towerPosition = position;
		
		//Gegner in Reichweite des Turmes ist, dann wird er zur Collection hinzugefügt
		if (abs(enemyPosition.x - (position.x + 0.5f)) - 0.5f <= type.range && 
			abs(enemyPosition.y - (position.y + 0.5f)) - 0.5f <= type.range)
			targets.push_back(*it);
	}

	//Wenn kein Slow-Down Turm, dann erstes Ziel nehmen
	if (this->getType().slow == 0.0f)
	{
		if (targets.size() > 0)
			target = targets[0];

		return target;
	}

	//Erster Gegner der nicht verlangsamt ist, wird als Ziel festgelegt
	for (std::vector<Enemy*>::iterator it = targets.begin(); it != targets.end() && target == 0; ++it)
	{
		if (!(*it)->isSlowed())
			target = *it;
	}

	//Ansonsten wird immer der erste Gegner als Ziel festgelegt
	if (target == 0 && targets.size() > 0)
		target = targets[0];

	return target;
}

//Liefert die Position des Turmes
const CL_Point& Tower::getPosition() const
{
	return position;
}

//Liefert den Typ des Turmes
const TowerType& Tower::getType() const
{
	return type;
}

//Liefert die aktuell fliegenden Geschosse
const std::vector<Missile*>& Tower::getMissiles() const
{
	return missiles;
}