#include "missile.h"
#include "world.h"

Missile::Missile(World *world, const CL_Pointx<float>& position, Enemy& target, float damage, float slow, float splash, const CL_Color& color)
: GameObject(world), position(position), target(target), damage(damage), slow(slow), splash(splash), color(color)
{
}

// aktualisiert die Missile
bool Missile::update(int timeElapsed_ms)
{
	// Vektor von aktueller Position zum Ziel berechnen
	CL_Vec2<float> distance = target.getPosition() - position;

	// zur�ckzulegende Distanz berechnen
	float movement = timeElapsed_ms * 0.01f;

	// wenn zur�ckzulegende Distanz gr��er als L�nge des Vektors
	if (movement > distance.length())
	{
		// Gegner treffen
		hitTargets();

		// Missile zerst�ren
		return false;
	}
	// sonst: Position entlang des Vektors um zur�ckzulegende Distanz anpassen
	else
		position += CL_Vec2<float>::normalize(distance) * movement;

	return true;
}

// liefert die aktuelle Position der Missile
const CL_Pointx<float>& Missile::getPosition() const
{
	return position;
}

// l�sst die Missile Gegner treffen
void Missile::hitTargets()
{
	// das Prim�rziel treffen
	target.hit(damage, slow, color);

	// wenn kein Fl�chenschaden -> Abbruch
	if (splash == 0)
		return;

	// Vektor aller Gegner aus der Spielwelt beziehen
	std::vector<Enemy*> enemies = world->getEnemies();

	// f�r alle Gegner
	for (std::vector<Enemy*>::const_iterator it = enemies.begin(); it != enemies.end(); ++it)
	{
		// wenn Genger tot oder Gegner ist Prim�rziel -> Gegner �berspringen
		if (!(*it)->isAlive() || (*it) == &target)
			continue;

		// Position von Prim�rziel und aktuellem Gegner berechnen
		CL_Pointx<float> enemyPosition = (*it)->getPosition();
		CL_Pointx<float> targetPosition = target.getPosition();

		// Entfernung zwischen Prim�rziel und aktuellem Gegner berechnen
		float distance = (enemyPosition - targetPosition).length();

		// wenn Entfernung kleiner gleich dem Radius des Fl�cheschadens
		if (distance <= splash)
		{
			// Verh�ltnis von Entfernung und Radius berechnen
			float ratio = 1.0f - distance / splash;

			// aktuellen Gegner mit angepasstem Schaden treffen
			(*it)->hit(damage * ratio, slow * ratio, color);
		}
	}
}